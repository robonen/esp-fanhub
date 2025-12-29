<template>
  <div class="bg-slate-800/50 rounded-lg p-2">
    <svg viewBox="0 0 300 150" class="w-full h-28">
      <!-- Grid -->
      <line x1="0" y1="150" x2="300" y2="150" stroke="#475569" stroke-width="0.5" />
      <line x1="0" y1="75" x2="300" y2="75" stroke="#475569" stroke-width="0.5" stroke-dasharray="4" />
      <line x1="0" y1="0" x2="300" y2="0" stroke="#475569" stroke-width="0.5" />

      <!-- Curve -->
      <polyline :points="data.polyline" fill="none" stroke="#34d399" stroke-width="2" />

      <!-- Current temp indicator -->
      <line
        :x1="data.currentX"
        y1="0"
        :x2="data.currentX"
        y2="150"
        stroke="#f59e0b"
        stroke-width="1.5"
        stroke-dasharray="4"
      />
      <circle :cx="data.currentX" :cy="data.currentY" r="4" fill="#f59e0b" />

      <!-- Labels -->
      <text x="5" y="145" fill="#64748b" font-size="10">{{ data.minT }}°</text>
      <text x="260" y="145" fill="#64748b" font-size="10">{{ data.maxT }}°</text>
      <text x="5" y="12" fill="#64748b" font-size="10">{{ data.maxD }}%</text>
      <text x="270" y="12" fill="#64748b" font-size="8">{{ data.minD }}%</text>
    </svg>
  </div>
</template>

<script setup lang="ts">
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
  data: GraphData;
}>();
</script>
