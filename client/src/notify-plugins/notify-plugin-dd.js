class NotifyPluginDD {
    onWork() {
        console.log('dd on work')
    }

    onBreak() {
        console.log('dd on break')
    }

    onWorkEnd(force) {
        console.log('dd on workEnd force:' + force)
    }

    onBreakEnd(force) {
        console.log('dd on breakEnd force:' + force)
    }
}

export default NotifyPluginDD
