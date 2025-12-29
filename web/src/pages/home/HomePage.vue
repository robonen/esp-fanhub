<template>
  <div class="min-h-screen p-4">
    <div class="max-w-3xl mx-auto">
      <!-- Header -->
      <div class="mb-4 p-4 rounded-xl border border-slate-800 bg-slate-900/60">
        <h1 class="text-xl font-semibold">ESP FanHub</h1>
        <div class="flex items-center gap-4 mt-2">
          <p class="text-sm">
            <span class="text-slate-400">Wi-Fi:</span>
            <span :class="wifi ? 'text-emerald-400' : 'text-red-400'" class="ml-1">
              {{ wifi ? 'Подключено' : 'Подключение...' }}
            </span>
            <span v-if="wifi && ip" class="text-slate-500 ml-1">({{ ip }})</span>
          </p>
          <p class="text-sm">
            <span class="text-slate-400">Температура:</span>
            <span class="font-mono text-amber-400 ml-1">{{ tempC.toFixed(1) }}°C</span>
          </p>
        </div>
      </div>

      <!-- Fans -->
      <div class="grid md:grid-cols-3 gap-4">
        <FanCard
          v-for="(fan, i) in fans"
          :key="i"
          :fan-index="i"
          :duty-pct="fan.dutyPct"
          :manual="fan.manual"
          :manual-duty="manualDuty[i]"
          :points="points[i]"
          :graph-data="graphData[i]"
          @mode-change="setMode(i, $event)"
          @duty-change="manualDuty[i] = $event"
          @duty-save="setManualDuty(i, manualDuty[i])"
          @points-change="points[i] = $event"
          @point-add="addPoint(i)"
          @point-remove="removePoint(i, $event)"
          @points-save="savePoints(i)"
          @edit-start="isEditing = true"
          @edit-end="isEditing = false"
        />
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { FanCard, useFanController, useFanGraph } from '../../modules/fan-control';

const {
  wifi,
  ip,
  tempC,
  fans,
  manualDuty,
  points,
  isEditing,
  setMode,
  setManualDuty,
  savePoints,
  addPoint,
  removePoint,
} = useFanController();

const graphData = useFanGraph(points, fans, tempC);
</script>
