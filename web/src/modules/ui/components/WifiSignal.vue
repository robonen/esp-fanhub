<template>
  <div class="inline-flex items-center gap-1 text-sm">
    <component
      :is="icon"
      class="size-4"
      :class="iconClass"
      aria-hidden="true"
    />
    <span class="font-mono text-xs text-text-secondary">{{ display }}</span>
  </div>
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
