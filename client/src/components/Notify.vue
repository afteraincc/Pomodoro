<script>
export default {
  data() {
    return {
      timer: '00:00',
    }
  },
  mounted() {
    initSelf(this)
  },
}

let self = null
function initSelf(s) {
  self = s
}
</script>

<script setup>
document.title = 'Notify'

window.electronAPI.handleNotifyTimer((event, t) => {
  self.timer = t
})
</script>


<template>
  <div>
    <van-divider>休息时间</van-divider>
    <van-row justify="center">
      <div style="font-size: 96px;">{{ timer }}</div>
    </van-row>
  </div>
</template>

<style></style>
