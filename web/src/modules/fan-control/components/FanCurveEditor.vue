<template>
  <div class="space-y-2">
    <div class="text-xs text-slate-400 mb-2">Кривая (Темп → Скорость)</div>

    <!-- Graph -->
    <FanCurveGraph :data="graphData" />

    <!-- Points Editor -->
    <div class="space-y-1 max-h-32 overflow-y-auto">
      <div v-for="(pt, j) in points" :key="j" class="flex items-center gap-1">
        <input
          type="number"
          class="w-14 px-1.5 py-0.5 rounded bg-slate-800 text-xs text-center"
          :value="pt.temp"
          @input="onTempChange(j, $event)"
          @focus="$emit('edit-start')"
          @blur="$emit('edit-end')"
        />
        <span class="text-xs text-slate-500">°→</span>
        <input
          type="number"
          class="w-14 px-1.5 py-0.5 rounded bg-slate-800 text-xs text-center"
          :value="pt.duty"
          @input="onDutyChange(j, $event)"
          @focus="$emit('edit-start')"
          @blur="$emit('edit-end')"
        />
        <span class="text-xs text-slate-500">%</span>
        <button
          class="px-1.5 py-0.5 rounded bg-slate-800 hover:bg-red-900/50 text-xs text-slate-400 hover:text-red-400"
          @click="$emit('remove', j)"
        >
          ✕
        </button>
      </div>
    </div>

    <!-- Actions -->
    <div class="flex gap-2 pt-2">
      <button
        class="flex-1 px-2 py-1 rounded bg-slate-800 hover:bg-slate-700 text-xs"
        @click="$emit('add')"
      >
        + Точка
      </button>
      <button
        class="flex-1 px-2 py-1 rounded bg-emerald-700 hover:bg-emerald-600 text-xs"
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
