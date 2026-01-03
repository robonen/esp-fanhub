<template>
  <div class="space-y-3">
    <div class="flex items-center justify-between gap-2">
      <span class="text-xs text-text-secondary">Кривая: кликните на график для добавления точки</span>
      <button
        class="text-xs text-text-muted hover:text-accent-info transition flex items-center gap-1"
        title="Скопировать кривую на остальные вентиляторы"
        @click="syncCurvesToCurrent"
      >
        <Copy class="size-3" aria-hidden="true" />
        <span>Синхр.</span>
      </button>
    </div>

    <!-- Graph -->
    <FanCurveGraph :data="graphData" @click="onGraphClick" />

    <!-- Points as tags -->
    <div class="flex flex-wrap gap-1.5">
      <div 
        v-for="(pt, j) in sortedPoints" 
        :key="j" 
        class="inline-flex items-center gap-1 px-2 py-1 bg-surface-overlay rounded text-xs"
      >
        <span class="text-text-primary font-mono">{{ pt.temp }}°→{{ pt.duty }}%</span>
        <button
          class="text-text-muted hover:text-accent-danger transition"
          @click="removePoint(fanIndex, pt.originalIndex)"
        >
          <X class="size-3" aria-hidden="true" />
        </button>
      </div>
      <span v-if="fanPoints.length === 0" class="text-xs text-text-muted italic py-1">
        Нет точек
      </span>
    </div>

    <!-- Save button -->
    <button
      v-if="hasChanges"
      class="w-full px-3 py-2 rounded-md bg-accent-success text-surface-base text-sm transition hover:brightness-110 font-medium"
      @click="savePoints(fanIndex)"
    >
      Сохранить кривую
    </button>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue';
import { X, Copy } from 'lucide-vue-next';
import { useFanController } from '../composables/useFanController';
import { useFanGraph } from '../composables/useFanGraph';
import FanCurveGraph from './FanCurveGraph.vue';

const props = defineProps<{
  fanIndex: number;
}>();

const { points, hasUnsavedChanges, addPoint, removePoint, savePoints, updatePoints, syncCurvesToCurrent } = useFanController();

const fanPoints = computed(() => points.value[props.fanIndex] ?? []);
const hasChanges = computed(() => hasUnsavedChanges.value[props.fanIndex]);

const graphData = useFanGraph(() => props.fanIndex);

const sortedPoints = computed(() => {
  return fanPoints.value
    .map((pt, i) => ({ ...pt, originalIndex: i }))
    .sort((a, b) => a.temp - b.temp);
});

function onGraphClick(temp: number, duty: number) {
  const current = fanPoints.value;
  const newPoints = [...current, { temp, duty }].sort((a, b) => a.temp - b.temp);
  updatePoints(props.fanIndex, newPoints);
}
</script>
