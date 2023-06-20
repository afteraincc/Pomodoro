const { ipcRenderer } = require('electron')

window.electronAPI = {
  cancelBluetoothRequest: (callback) => ipcRenderer.send('cancel-bluetooth-request', callback),
  notifyWithTopWindow: () => ipcRenderer.send('notify-with-top-window'),
  cancelNotify: () => ipcRenderer.send('cancel-notify'),
  notifyTimer: (timer) => ipcRenderer.send('notify-timer', timer),
  handleNotifyTimer: (callback) => ipcRenderer.on('notify-timer', callback),
  openProcess: (processName) => ipcRenderer.send('open-process', processName),
  killProcess: (processName) => ipcRenderer.send('kill-process', processName),
  isWin32: () => ipcRenderer.invoke('isWin32'),
}