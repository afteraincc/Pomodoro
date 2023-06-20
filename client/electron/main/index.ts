import { app, BrowserWindow, shell, ipcMain } from 'electron'
const path = require('path')

// The built directory structure
//
// ├─┬ dist-electron
// │ ├─┬ main
// │ │ └── index.js    > Electron-Main
// │ └─┬ preload
// │   └── index.js    > Preload-Scripts
// ├─┬ dist
// │ └── index.html    > Electron-Renderer
//
process.env.DIST_ELECTRON = path.join(__dirname, '..')
process.env.DIST = path.join(process.env.DIST_ELECTRON, '../dist')
process.env.PUBLIC = process.env.VITE_DEV_SERVER_URL
  ? path.join(process.env.DIST_ELECTRON, '../public')
  : process.env.DIST

// Remove electron security warnings
// This warning only shows in development mode
// Read more on https://www.electronjs.org/docs/latest/tutorial/security
// process.env['ELECTRON_DISABLE_SECURITY_WARNINGS'] = 'true'

let win: BrowserWindow | null = null
let selectBluetoothCallback

const url = process.env.VITE_DEV_SERVER_URL
const indexHtml = path.join(process.env.DIST, 'index.html')

function createWindow() {
  win = new BrowserWindow({
    title: '',
    width: 550, height: 650,
    webPreferences: {
      preload: path.join(__dirname, '../preload/index.js'),
      // Warning: Enable nodeIntegration and disable contextIsolation is not secure in production
      // Consider using contextBridge.exposeInMainWorld
      // Read more on https://www.electronjs.org/docs/latest/tutorial/context-isolation
      nodeIntegration: true,
      contextIsolation: false,
    },
  })

  if (process.env.VITE_DEV_SERVER_URL) { // electron-vite-vue#298
    win.loadURL(url)
    // Open devTool if the app is not packaged
    win.webContents.openDevTools()
  } else {
    win.loadFile(indexHtml)
  }

  // Make all links open with the browser, not with the application
  win.webContents.setWindowOpenHandler(({ url }) => {
    if (url.startsWith('https:')) shell.openExternal(url)
    return { action: 'deny' }
  })

  win.webContents.on('select-bluetooth-device', (event, deviceList, callback) => {
    event.preventDefault()
    selectBluetoothCallback = callback

    if (deviceList.length > 0) {
      callback(deviceList[0].deviceId)
    } else {
      // The device wasn't found so we need to either wait longer (eg until the
      // device is turned on) or until the user cancels the request
    }
  })

  configIPCMain()
}

app.whenReady().then(() => {
  createWindow()

  app.on('activate', function () {
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') app.quit()
})

function findWindow(title) {
  var retWin = null
  BrowserWindow.getAllWindows().forEach(w => {
    if (w.title == title) {
      retWin = w
    }
  })
  return retWin
}

function createNotfiyWindow() {
  var notifyWin = new BrowserWindow({
    title: '',
    width: 800, height: 600,
    closable: false, maximizable: false, minimizable: false,
    alwaysOnTop: true,
    webPreferences: {
      preload: path.join(__dirname, '../preload/index.js'),
      // Warning: Enable nodeIntegration and disable contextIsolation is not secure in production
      // Consider using contextBridge.exposeInMainWorld
      // Read more on https://www.electronjs.org/docs/latest/tutorial/context-isolation
      nodeIntegration: true,
      contextIsolation: false,
    },
  })

  if (process.env.VITE_DEV_SERVER_URL) { // electron-vite-vue#298
    notifyWin.loadURL(url + "#/notify")
  } else {
    notifyWin.loadFile(indexHtml, { hash: "#/notify" })
  }
}

function configIPCMain() {
  ipcMain.on('cancel-bluetooth-request', (event) => {
    selectBluetoothCallback('')
  })

  ipcMain.on('notify-with-top-window', (event) => {
    var w = findWindow('Notify')
    if (null == w) {
      createNotfiyWindow()
    }
  })

  ipcMain.on('cancel-notify', (event) => {
    var w = findWindow('Notify')
    if (null != w) {
      w.destroy()
    }
  })

  ipcMain.on('notify-timer', (event, timer) => {
    var w = findWindow('Notify')
    if (null != w) {
      w.webContents.send('notify-timer', timer)
    }
  })

  ipcMain.on('open-process', (event, processName) => {
    var childProcess = require('child_process')
    var exec = childProcess.exec

    switch (process.platform) {
      case 'win32':
        exec(processName)
        break
      default: //Linux + Darwin
        exec('open -a ' + processName)
        break
    }
  })

  ipcMain.on('kill-process', (event, processName) => {
    var childProcess = require('child_process')
    var exec = childProcess.exec

    switch (process.platform) {
      case 'win32':
        exec('taskkill /F /IM ' + processName + '.exe /T')
        break
      default: //Linux + Darwin
        exec('pkill -f ' + processName)
        break
    }
  })

  ipcMain.handle('isWin32', () => {
    if ('win32' == process.platform) {
      return true
    } else {
      return false
    }
  })
}