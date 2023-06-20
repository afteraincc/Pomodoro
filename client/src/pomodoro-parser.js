class StatusParser {
    constructor(buffer) {
        this.buffer = buffer;
    }

    _getPomodoroStatus() {
        return this.buffer.getUint16(0, /*littleEndian=*/true)
    }

    _getPomodoroTotal() {
        return this.buffer.getUint16(2, /*littleEndian=*/true)
    }

    _getPomodoroSecond() {
        return this.buffer.getUint16(4, /*littleEndian=*/true)
    }

    getMinute() {
        return ("00" + parseInt((this._getPomodoroTotal() - this._getPomodoroSecond()) / 60)).slice(-2)
    }

    getSecond() {
        return ("00" + parseInt((this._getPomodoroTotal() - this._getPomodoroSecond()) % 60)).slice(-2)
    }

    isTimeout() {
        return (this._getPomodoroSecond() > this._getPomodoroTotal())
    }

    isWorkStatus() {
        return (0 == this._getPomodoroStatus())
    }

    isBreakStatus() {
        return (1 == this._getPomodoroStatus())
    }

    isIdleStatus() {
        return (2 == this._getPomodoroStatus())
    }
}

export default StatusParser
