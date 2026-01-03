<template>
  <div class="p-4 rounded-xl border border-border-muted bg-surface-elevated/60">
    <div class="flex items-center justify-between mb-3">
      <h2 class="font-medium">Вентилятор {{ fanIndex + 1 }}</h2>
      <span class="text-xs text-text-secondary">{{ dutyPct.toFixed(0) }}%</span>
    </div>

    <!-- Mode Toggle -->
    <FanModeToggle :manual="manual" class="mb-3" @change="$emit('mode-change', $event)" />

    <!-- Manual Mode -->
    <FanManualControl
      v-if="manual"
      :model-value="manualDuty"
      @update:model-value="$emit('duty-change', $event)"
      @save="$emit('duty-save')"
    />

    <!-- Auto Mode (Curve) -->
    <FanCurveEditor
      v-else
      :points="points"
      :graph-data="graphData"
      @update:points="$emit('points-change', $event)"
      @add="$emit('point-add')"
      @remove="$emit('point-remove', $event)"
      @save="$emit('points-save')"
      @edit-start="$emit('edit-start')"
      @edit-end="$emit('edit-end')"
    />
  </div>
</template>

<script setup lang="ts">
import type { CurvePoint } from '../../../common/types';
import FanModeToggle from './FanModeToggle.vue';
import FanManualControl from './FanManualControl.vue';
import FanCurveEditor from './FanCurveEditor.vue';

interface GraphData {
  minT: number;
  maxT: number;
  minD: number;
  maxD: number;
  polyline: string;
  currentX: number;
  currentY: number;
}

defineProps<{
  fanIndex: number;
  dutyPct: number;
  manual: boolean;
  manualDuty: number;
  points: CurvePoint[];
  graphData: GraphData;
}>();

defineEmits<{
  'mode-change': [manual: boolean];
  'duty-change': [duty: number];
  'duty-save': [];
  'points-change': [points: CurvePoint[]];
  'point-add': [];
  'point-remove': [index: number];
  'points-save': [];
  'edit-start': [];
  'edit-end': [];
}>();
</script>
