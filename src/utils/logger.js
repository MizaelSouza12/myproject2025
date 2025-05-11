const chalk = require('chalk');

class Logger {
  static LEVELS = {
    DEBUG: 0,
    INFO: 1,
    WARN: 2,
    ERROR: 3
  };

  constructor(options = {}) {
    this.level = options.level || Logger.LEVELS.INFO;
    this.prefix = options.prefix || 'WYDStudio';
  }

  format(level, message) {
    const timestamp = new Date().toISOString();
    const prefix = chalk.cyan(`[${this.prefix}]`);
    const levelStr = this.getLevelString(level);
    return `${prefix} ${levelStr} ${message}`;
  }

  getLevelString(level) {
    switch(level) {
      case Logger.LEVELS.DEBUG:
        return chalk.gray('[DEBUG]');
      case Logger.LEVELS.INFO:
        return chalk.blue('[INFO]');
      case Logger.LEVELS.WARN:
        return chalk.yellow('[WARN]');
      case Logger.LEVELS.ERROR:
        return chalk.red('[ERROR]');
      default:
        return chalk.white('[LOG]');
    }
  }

  debug(message) {
    if (this.level <= Logger.LEVELS.DEBUG) {
      console.log(this.format(Logger.LEVELS.DEBUG, message));
    }
  }

  info(message) {
    if (this.level <= Logger.LEVELS.INFO) {
      console.log(this.format(Logger.LEVELS.INFO, message));
    }
  }

  warn(message) {
    if (this.level <= Logger.LEVELS.WARN) {
      console.warn(this.format(Logger.LEVELS.WARN, message));
    }
  }

  error(message) {
    if (this.level <= Logger.LEVELS.ERROR) {
      console.error(this.format(Logger.LEVELS.ERROR, message));
    }
  }
}

module.exports = Logger; 