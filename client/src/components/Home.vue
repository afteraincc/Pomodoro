<script setup>
document.title = 'Pomodoro Client'
</script>
<script>
import StatusParser from '../pomodoro-parser.js'
import NotifyPluginManager from '../notify-plugin-manager.js'
import { parserOptions } from '@vue/compiler-dom';

export default {
  data() {
    return {
      timer: '00:00',
      connectBtnDisabled: false,
      cancelBtntDisabled: true,
      disconnecBtnDisabled: true,
      setupBtnDisabled: true,
      cancelNotifyWinBtnDisabled: false,
      interval: 25,
      execNotifyPlugin: false,
      weChatWin32Path: 'C:\\Program Files (x86)\\Tencent\\WeChat\\WeChat.exe',
      dingTalkWin32Path: 'C:\\Program Files (x86)\\DingDing\\DingtalkLauncher.exe',
    }
  },
  mounted() {
    initSelf(this)
    defaultStatus()
    initNotifyPlugin()

    if (localStorage.weChatWin32Path) {
      this.weChatWin32Path = localStorage.weChatWin32Path
    }
    if (localStorage.dingTalkWin32Path) {
      this.dingTalkWin32Path = localStorage.dingTalkWin32Path
    }
    if (localStorage.execNotifyPlugin) {
      this.execNotifyPlugin = localStorage.execNotifyPlugin === 'true'
    }
  },
  watch: {
    weChatWin32Path(newWeChatWin32Path) {
      localStorage.weChatWin32Path = newWeChatWin32Path
    },
    dingTalkWin32Path(newDingTalkWin32Path) {
      localStorage.dingTalkWin32Path = newDingTalkWin32Path
    },
    execNotifyPlugin(newExecNotifyPlugin) {
      if (newExecNotifyPlugin)
        localStorage.execNotifyPlugin = 'true'
      else
        localStorage.execNotifyPlugin = 'false'
    }
  },
  methods: {
    connectDevice(event) {
      connectDeviceImpl()
    },
    cancelDevice(event) {
      cancelDeviceImpl()
    },
    disconnectDevice(event) {
      disconnectDeviceImpl()
    },
    setupInterval(event) {
      setupIntervalImpl()
    },
    cancelNotifyWin(event) {
      cancelNotifyWinImpl()
    },
    openProcess(event) {
      openProcessImpl()
    },
    killProcess(event) {
      killProcessImpl()
    }
  }
}

let self = null
function initSelf(s) {
  self = s
}

let notifyPluginManager = null;
function initNotifyPlugin() {
  notifyPluginManager = new NotifyPluginManager()
  notifyPluginManager.reset()
  notifyPluginManager.autoload()
}

let device = null
let server = null
let service = null
let statusCharacteristic = null
let minuteCharacteristic = null

function defaultStatus() {
  self.connectBtnDisabled = false
  self.cancelBtntDisabled = true
  self.disconnecBtnDisabled = true
  self.setupBtnDisabled = true
}

function connectingStatus() {
  self.connectBtnDisabled = true
  self.cancelBtntDisabled = false
  self.disconnecBtnDisabled = true
  self.setupBtnDisabled = true
}

function connectedStatus() {
  self.connectBtnDisabled = true
  self.cancelBtntDisabled = true
  self.disconnecBtnDisabled = false
  self.setupBtnDisabled = false
}

function cannelStatus() {
  defaultStatus()
}

function disconnectStatus() {
  defaultStatus()
}

async function connectDeviceImpl() {
  connectingStatus()
  device = await navigator.bluetooth.requestDevice({
    filters: [
      { name: 'Pomodoro' },
      { services: ['221827f2-4cc5-483b-8df0-866bd41bd2a2'] },
    ]
  })
  connectedStatus()
  device.addEventListener('gattserverdisconnected', onDeviceDisconnected)
  server = await device.gatt.connect()
  service = await server.getPrimaryService('221827f2-4cc5-483b-8df0-866bd41bd2a2')
  statusCharacteristic = await service.getCharacteristic('22390b44-2557-4b2e-a467-70a18d02591e')
  minuteCharacteristic = await service.getCharacteristic('f7b560f0-6626-4a7b-b6d8-2d2c11f85441')

  notifyPluginManager.reset()
  statusCharacteristic.startNotifications().then(characteristic => {
    characteristic.addEventListener('characteristicvaluechanged', e => {
      var parser = new StatusParser(e.target.value.buffer ? e.target.value : new DataView(e.target.value))

      if (!parser.isTimeout()) {
        self.timer = parser.getMinute() + ":" + parser.getSecond()

        if (parser.isWorkStatus()) {
          if (self.execNotifyPlugin)
            notifyPluginManager.work()
          window.electronAPI.cancelNotify()
        } else if (parser.isBreakStatus()) {
          if (self.execNotifyPlugin)
            notifyPluginManager.break()
          window.electronAPI.notifyWithTopWindow()
          window.electronAPI.notifyTimer(self.timer)
        } else if (parser.isIdleStatus()) {
          window.electronAPI.cancelNotify()
        }
      } else {
        if (parser.isWorkStatus()) {
          if (self.execNotifyPlugin)
            notifyPluginManager.workEnd()
          window.electronAPI.notifyWithTopWindow()
        } else if (parser.isBreakStatus()) {
          if (self.execNotifyPlugin)
            notifyPluginManager.breakEnd()
          window.electronAPI.cancelNotify()
        }
      }
    })
  })
}

function cancelDeviceImpl() {
  window.electronAPI.cancelBluetoothRequest()
  cannelStatus()
}

function disconnectDeviceImpl() {
  device.gatt.disconnect()
  disconnectStatus()
}

function setupIntervalImpl() {
  let i = new Uint8Array(self.interval.toString().split('').map(c => c.charCodeAt()))

  minuteCharacteristic.writeValue(i).then(() => {
    console.log('setup interval done')
  })
}

function onDeviceDisconnected() {
  window.electronAPI.cancelNotify()
  disconnectDeviceImpl()
}

function cancelNotifyWinImpl() {
  window.electronAPI.cancelNotify()
}

function openProcessImpl() {
  window.electronAPI.isWin32().then((isWin32) => {
    if (isWin32) {
      window.electronAPI.openProcess('"' + self.weChatWin32Path + '"')
      window.electronAPI.openProcess('"' + self.dingTalkWin32Path + '"')
    } else {
      window.electronAPI.openProcess('WeChat')
      window.electronAPI.openProcess('DingTalk')
    }
  })
}

function killProcessImpl() {
  window.electronAPI.killProcess('WeChat')
  window.electronAPI.killProcess('DingTalk')
}
</script>

<template>
  <div>
    <van-divider>设备</van-divider>
    <van-row justify="center">
      <van-space>
        <van-button plain type='primary' :disabled="connectBtnDisabled" @click="connectDevice">连接设备</van-button>
        <van-button plain type='primary' :disabled="cancelBtntDisabled" @click="cancelDevice">连接中...取消</van-button>
        <van-button plain type='primary' :disabled="disconnecBtnDisabled" @click="disconnectDevice">断开设备</van-button>
      </van-space>
    </van-row>
  </div>

  <div>
    <van-divider>倒计时</van-divider>
    <van-row justify="center">
      <van-space>
        <van-field v-model="timer" :disabled="true" label-align="right" label="工作时间"></van-field>
      </van-space>
    </van-row>
  </div>


  <div>
    <van-divider>设置(单位：分钟)</van-divider>
    <van-row justify="center">
      <van-space>
        <van-button plain type='primary' :disabled="setupBtnDisabled" @click="setupInterval">设置工作时间</van-button>
        <van-stepper v-model="interval" :disabled="setupBtnDisabled" integer theme="round" default-value="25" min="1"
          max="60" />
      </van-space>
    </van-row>
  </div>

  <div>
    <van-divider>特殊</van-divider>
    <van-row justify="center">
      <van-space>
        <van-button :disabled="cancelNotifyWinBtnDisabled" @click="cancelNotifyWin">关闭通知窗口</van-button>
        <van-button @click="openProcess">打开进程</van-button>
        <van-button @click="killProcess">关闭进程</van-button>
      </van-space>
    </van-row>

    <van-divider>通知插件管理</van-divider>
    <van-row justify="center">
      <van-checkbox v-model="execNotifyPlugin">是否执行插件</van-checkbox>
    </van-row>

    <van-divider>Windows程序路径配置(自动保存)</van-divider>
    <van-cell-group inset>
      <van-field v-model="weChatWin32Path" label="微信"
        placeholder='C:\Program Files (x86)\Tencent\WeChat\WeChat.exe'></van-field>
      <van-field v-model="dingTalkWin32Path" label="钉钉"
        placeholder='C:\Program Files (x86)\DingDing\DingtalkLauncher.exe'></van-field>
    </van-cell-group>
  </div>
</template>

<style></style>