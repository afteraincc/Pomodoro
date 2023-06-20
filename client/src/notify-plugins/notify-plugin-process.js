class NotifyPluginProcess {
    onWork() {
        console.log('process on work')
        window.electronAPI.killProcess('WeChat')
        window.electronAPI.killProcess('DingTalk')
    }

    onBreak() {
        console.log('process on break')
        window.electronAPI.killProcess('WeChat')
        window.electronAPI.killProcess('DingTalk')
    }

    onWorkEnd(force) {
        console.log('process on workEnd force:' + force)
        if (force) {
            window.electronAPI.isWin32().then((isWin32) => {
                if (isWin32) {
                    window.electronAPI.openProcess('"' + localStorage.weChatWin32Path + '"')
                    window.electronAPI.openProcess('"' + localStorage.dingTalkWin32Path + '"')
                } else {
                    window.electronAPI.openProcess('WeChat')
                    window.electronAPI.openProcess('DingTalk')
                }
            })
        }
    }

    onBreakEnd(force) {
        console.log('process on breakEnd force:' + force)
        window.electronAPI.isWin32().then((isWin32) => {
            if (isWin32) {
                window.electronAPI.openProcess('"' + localStorage.weChatWin32Path + '"')
                window.electronAPI.openProcess('"' + localStorage.dingTalkWin32Path + '"')
            } else {
                window.electronAPI.openProcess('WeChat')
                window.electronAPI.openProcess('DingTalk')
            }
        })
    }
}

export default NotifyPluginProcess
