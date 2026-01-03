<template>
  <div class="space-y-2">
    <div class="text-xs text-text-secondary mb-2">Кривая (Темп → Скорость)</div>

    <!-- Graph -->
    <FanCurveGraph :data="graphData" />

    <!-- Points Editor -->
    <div class="space-y-1 max-h-32 overflow-y-auto">
      <div v-for="(pt, j) in points" :key="j" class="flex items-center gap-1">
        <input
          type="number"
          class="w-14 px-2 py-1 rounded-md bg-surface-overlay text-xs text-center focus:outline focus:outline-border-default"
          :value="pt.temp"
          @input="onTempChange(j, $event)"
          @focus="$emit('edit-start')"
          @blur="$emit('edit-end')"
        />
        <span class="text-xs text-text-muted">°→</span>
        <input
          type="number"
          class="w-14 px-2 py-1 rounded-md bg-surface-overlay text-xs text-center focus:outline focus:outline-border-default"
          :value="pt.duty"
          @input="onDutyChange(j, $event)"
          @focus="$emit('edit-start')"
          @blur="$emit('edit-end')"
        />
        <span class="text-xs text-text-muted">%</span>
        <button
          class="px-2 py-1 rounded-md bg-interactive-default text-xs text-text-secondary transition hover:bg-accent-danger/30 hover:text-accent-danger"
          @click="$emit('remove', j)"
        >
          ✕
        </button>
      </div>
    </div>

    <!-- Actions -->
    <div class="flex gap-2 pt-2">
      <button
        class="flex-1 px-2 py-1 rounded-md bg-interactive-default text-xs transition hover:bg-interactive-hover"
        @click="$emit('add')"
      >
        + Точка
      </button>
      <button
        class="flex-1 px-2 py-1 rounded-md bg-accent-success text-surface-base text-xs transition hover:brightness-110"
        @click="$emit('save')"
      >
        Сохранить
      </button>
    </div>
  </div>
</template>

<script setup lang="ts">
import type { CurvePoint } from '../../../common/types';
import FanCurveGraph from './FanCurveGraph.vue';

interface GraphData {
  minT: number;
  maxT: number;
  minD: number;
  maxD: number;
  polyline: string;
  currentX: number;
  currentY: number;
}

const emit = defineEmits<{
  'update:points': [points: CurvePoint[]];
  add: [];
  remove: [index: number];
  save: [];
  'edit-start': [];
  'edit-end': [];
}>();

const props = defineProps<{
  points: CurvePoint[];
  graphData: GraphData;
}>();

function onTempChange(index: number, event: Event) {
  const target = event.target as HTMLInputElement;
  emit('update:points', updatePoint(index, { temp: Number(target.value) }));
}

function onDutyChange(index: number, event: Event) {
  const target = event.target as HTMLInputElement;
  emit('update:points', updatePoint(index, { duty: Number(target.value) }));
}

function updatePoint(index: number, update: Partial<CurvePoint>): CurvePoint[] {
  return props.points.map((p, i) => (i === index ? { ...p, ...update } : p));
}
</script>
