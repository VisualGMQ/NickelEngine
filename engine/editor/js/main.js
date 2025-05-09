const { app, BrowserWindow, ipcMain } = require('electron/main')

const createWindow = () => {
  const win = new BrowserWindow({
    width: 2048,
    height: 1024,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
    }
  })

  win.loadFile('html/index.html')
  return win
}

app.whenReady().then(() => {
  let win = createWindow()
  ipcMain.handle('getNativeWindowHandle', () => win.getNativeWindowHandle())
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
})