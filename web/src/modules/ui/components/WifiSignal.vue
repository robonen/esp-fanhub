<template>
  <component
    :is="icon"
    class="size-4"
    :class="iconClass"
    :title="display"
    aria-hidden="true"
  />
</template>

<script setup lang="ts">
import { computed } from 'vue';
import { Wifi, WifiOff, SignalLow, SignalMedium, SignalHigh } from 'lucide-vue-next';

const props = defineProps<{
  wifi: boolean;
  rssi: number;
}>();

const bars = [
  { min: -55, icon: SignalHigh, klass: 'text-accent-success' },
  { min: -65, icon: SignalMedium, klass: 'text-accent-info' },
  { min: -75, icon: SignalLow, klass: 'text-accent-warning' },
];

const icon = computed(() => {
  if (!props.wifi) return WifiOff;
  const hit = bars.find((b) => props.rssi >= b.min);
  return hit ? hit.icon : Wifi;
});

const iconClass = computed(() => {
  if (!props.wifi) return 'text-accent-danger';
  const hit = bars.find((b) => props.rssi >= b.min);
  return hit ? hit.klass : 'text-text-secondary';
});

const display = computed(() => {
  if (!props.wifi) return 'нет связи';
  return `${props.rssi} dBm`;
});
</script>
