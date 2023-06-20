class NotifyPluginWX {
    onWork() {
        console.log('wx on work')
    }

    onBreak() {
        console.log('wx on break')
    }

    onWorkEnd(force) {
        console.log('wx on workEnd force:' + force)
    }

    onBreakEnd(force) {
        console.log('wx on breakEnd force:' + force)
    }
}

export default NotifyPluginWX