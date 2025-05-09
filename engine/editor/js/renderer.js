const { contextBridge, ipcRenderer } = require('electron')
const addon = require('../addon/editor')

async function initEngine() {
    let handle = await ipcRenderer.invoke('getNativeWindowHandle')
    addon.initEngine(handle)
}

initEngine().then(()=> {
    UpdateEngine()
})

function UpdateEngine() {
    console.log("here")
    addon.updateEngine()
    requestAnimationFrame(UpdateEngine)
}