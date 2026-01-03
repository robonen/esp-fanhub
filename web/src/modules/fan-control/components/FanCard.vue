<template>
  <div class="p-4 rounded-xl border border-border-muted bg-surface-elevated/60">
    <div class="flex items-center justify-between mb-3">
      <h2 class="font-medium">Вентилятор {{ fanIndex + 1 }}</h2>
      <span class="text-xs text-text-secondary">{{ fan.dutyPct.toFixed(0) }}%</span>
    </div>

    <!-- Mode Toggle -->
    <FanModeToggle :manual="fan.manual" class="mb-4" @change="setMode(fanIndex, $event)" />

    <!-- Manual Mode -->
    <FanManualControl v-if="fan.manual" :fan-index="fanIndex" />

    <!-- Auto Mode (Curve) -->
    <FanCurveEditor v-else :fan-index="fanIndex" />
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue';
import { useFanController } from '../composables/useFanController';
import FanModeToggle from './FanModeToggle.vue';
import FanManualControl from './FanManualControl.vue';
import FanCurveEditor from './FanCurveEditor.vue';

const props = defineProps<{
  fanIndex: number;
}>();

const { fans, setMode } = useFanController();

const fan = computed(() => fans.value[props.fanIndex]);
</script>
