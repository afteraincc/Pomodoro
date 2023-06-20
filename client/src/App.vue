<script setup>
import { ref, computed } from 'vue'
import Home from './components/Home.vue'
import Notify from './components/Notify.vue'
import NotFound from './components/NotFound.vue'
const routes = {
  '/': Home,
  '/notify': Notify
}
const currentPath = ref(window.location.hash)
window.addEventListener('hashchange', () => {
  currentPath.value = window.location.hash
})
const currentView = computed(() => {
  return routes[currentPath.value.slice(1) || '/'] || NotFound
})
</script>
<template>
  <component :is="currentView" />
</template>