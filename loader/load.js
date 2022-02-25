/* eslint-disable no-console */

const fs = require('fs');
const path = require('path');
const SerialPort = require('serialport');
const highwayhash = require('highwayhash');
const parse = require('i40xx-asm');

const args = process.argv.slice(2);
const argsMap = Object.fromEntries(args.map((arg) => arg.replace(/^-/, '').split('=')));

// just 0's
const hashKey = Buffer.alloc(32);

const BAUD_RATE = 115200;
const DEFAULT_PORT_NAME = 'COM6';

const CMD_START = 'START\r\n';
const CMD_STOP = 'STOP\r\n';

/*
 * Send data to port
 */
const writeToPort = (port, data) => new Promise((resolve, reject) => {
  port.write(data, (err) => {
    if (err) {
      reject(err);
      return;
    }

    resolve();
  });
});

/*
 * Process received data
 */
const processIncomingData = (port) => {
  const MAX_CONTROL_SEQ_LENGTH = 20;
  // signal type for shutdown - either we have no data for a while ('nodata' mode) or when 0x0F is received in
  // binary mode ('controlbyte' mode)
  const shutdownMode = argsMap.shutdown || 'nodata';

  let lastChars = [];
  let isBinaryMode = false;
  let printedBinary = 0;
  let watchTimer = 0;
  let shutdownTimer = 0;
  let shutdowning = false;
  let startTs = 0;

  /*
   * Print received symbol either in ascii or binary mode
   *
   * Switch between modes based on control sequences
   */
  const printInputSymbol = (symbol) => {
    if (isBinaryMode) {
      process.stdout.write(`${symbol.toString(16).padStart(2, '0').toUpperCase()} `);
      printedBinary++;
      if (printedBinary === 16) {
        printedBinary = 0;
        process.stdout.write('\n');
      }
    } else {
      process.stdout.write(String.fromCharCode(symbol));
    }

    lastChars = [...lastChars, symbol].slice(-MAX_CONTROL_SEQ_LENGTH);

    // switch to ascii mode
    if (isBinaryMode && lastChars.slice(-4).map((pr) => pr.toString(16)).join('') === 'ffffffff') {
      isBinaryMode = false;
      process.stdout.write('\n');
    }

    // switch to binary mode
    if (!isBinaryMode && lastChars.slice(-12).map((pr) => String.fromCharCode(pr)).join('') === 'BINARY_START') {
      process.stdout.write('\n');
      isBinaryMode = true;
      printedBinary = 0;
    }
  };

  /*
   * Start shutdown routine
   */
  const startShutdown = () => {
    console.log(`Elapsed time: ${String(Math.floor((Date.now() - startTs) / 1000)).padStart(8, '0')}s`);
    shutdowning = true;
    console.log('\nSending STOP command');
    writeToPort(port, Buffer.from(CMD_STOP, 'utf8'));
    shutdownTimer = setTimeout(() => process.exit(), 1000);
  };

  port.on('data', async (data) => {
    startTs = startTs || Date.now();

    for (const symbol of data.values()) {
      printInputSymbol(symbol);
    }

    if (shutdowning) {
      if (shutdownTimer) {
        clearTimeout(shutdownTimer);
      }
      // delay actual shutdown if new characters occuried
      shutdownTimer = setTimeout(() => process.exit(), 1000);
    } else {
      // detect if we need to issue shutdown routine
      switch (shutdownMode) {
        case 'controlbyte':
          if (isBinaryMode && lastChars[lastChars.length - 1] === 0xF) {
            startShutdown();
          }
          break;
        case 'nodata':
          if (watchTimer) {
            clearTimeout(watchTimer);
          }
          watchTimer = setTimeout(() => startShutdown(), 1000);
          break;
        default:
          break;
      }
    }
  });
};

/*
 * Open port
 */
const openPort = (portName) => new Promise((resolve, reject) => {
  const port = new SerialPort(portName || DEFAULT_PORT_NAME, { baudRate: BAUD_RATE, rtscts: false }, (openErr) => {
    if (openErr) {
      reject(openErr);
      return;
    }

    port.on('error', (err) => console.log('Error: ', err.message || err));

    processIncomingData(port);

    resolve(port);
  });
});

/*
 * Build payload to be sent through UART to start program execution
 */
const buildStartCmdPayload = (data) => {
  const dataLen = Buffer.alloc(4);
  dataLen.writeInt32LE(data.length);
  return Buffer.concat([Buffer.from(CMD_START, 'utf8'), dataLen, data]);
};

/*
 * Function that either reads binary ROM or compiles ASM source code into binary
 */
const readROM = (filePath) => {
  const fileData = fs.readFileSync(filePath);
  if (filePath.endsWith('.bin')) {
    console.log('Using binary ROM');
    return fileData;
  }

  const hash = highwayhash.asHexString(hashKey, fileData);
  const hashKeyFileName = filePath.replace(/\.(.*)$/, '.hash');
  const binaryRomFileName = filePath.replace(/\.(.*)$/, '.bin');
  if (fs.existsSync(hashKeyFileName) && fs.existsSync(binaryRomFileName)) {
    const existingHash = fs.readFileSync(hashKeyFileName, 'utf-8');
    if (existingHash === hash) {
      console.log('Source code has not changed from last run, using existing ROM.');
      return fs.readFileSync(binaryRomFileName);
    }
  }

  console.log('Recompile source code...');
  const { errors, data } = parse(fileData.toString());
  if (Array.isArray(errors) && errors.length) {
    console.log('COULD NOT PARSE SOURCE CODE!');
    console.log(errors);
    process.exit(1);
  }

  const binaryRom = Buffer.from(data.buffer);
  fs.writeFileSync(hashKeyFileName, hash);
  fs.writeFileSync(binaryRomFileName, binaryRom);
  return binaryRom;
};

const main = async () => {
  const port = await openPort(argsMap.port);
  const rom = readROM(path.resolve(argsMap.rom));
  console.log('Sending START command');
  await writeToPort(port, buildStartCmdPayload(rom));
};

main()
  .catch((err) => console.error('Error', err.message, err));
