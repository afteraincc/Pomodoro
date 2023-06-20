class NotifyPluginManager {
    constructor() {
        this.notifyPlugins = new Array()

        this.reset()
    }

    register(notifyPlugin) {
        this.notifyPlugins.push(notifyPlugin)
    }

    async autoload() {
        var plugins = await import.meta.glob('./notify-plugins/*.js')
        var autoload = await import('./notify-plugins/autoload.js')
        var files = autoload.default
        for (let i = 0; i < files.length; i++) {
            for (const path in plugins) {
                if (path.indexOf(files[i]) != -1) {
                    plugins[path]().then((mod) => {
                        this.notifyPlugins.push(new mod.default)
                    })
                }
            }
        }
    }

    reset() {
        this.state = 'none'
    }

    idle() {
        if ('idle' == this.state) {
            return
        }

        for (let i = 0; i < this.notifyPlugins.length; i++) {
            if ('work' == this.state) {
                if (typeof this.notifyPlugins[i].onWorkEnd == 'function') {
                    this.notifyPlugins[i].onWorkEnd(true)
                }
            } else if ('break' == this.state) {
                if (typeof this.notifyPlugins[i].onBreakEnd == 'function') {
                    this.notifyPlugins[i].onBreakEnd(true)
                }
            }
        }
        this.state = 'idle'
    }

    work() {
        if ('work' == this.state) {
            return
        }

        this.state = 'work'
        for (let i = 0; i < this.notifyPlugins.length; i++) {
            if (typeof this.notifyPlugins[i].onWork == 'function') {
                this.notifyPlugins[i].onWork()
            }
        }
    }

    workEnd() {
        if ('idle' == this.state) {
            return
        }

        this.state = 'idle'
        for (let i = 0; i < this.notifyPlugins.length; i++) {
            if (typeof this.notifyPlugins[i].onWorkEnd == 'function') {
                this.notifyPlugins[i].onWorkEnd(false)
            }
        }
    }

    break() {
        if ('break' == this.state) {
            return
        }

        this.state = 'break'
        for (let i = 0; i < this.notifyPlugins.length; i++) {
            if (typeof this.notifyPlugins[i].onBreak == 'function') {
                this.notifyPlugins[i].onBreak()
            }
        }
    }

    breakEnd() {
        if ('idle' == this.state) {
            return
        }

        this.state = 'idle'
        for (let i = 0; i < this.notifyPlugins.length; i++) {
            if (typeof this.notifyPlugins[i].onBreakEnd == 'function') {
                this.notifyPlugins[i].onBreakEnd(false)
            }
        }
    }
}

export default NotifyPluginManager