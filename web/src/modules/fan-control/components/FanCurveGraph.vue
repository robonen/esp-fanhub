<template>
  <div class="bg-surface-overlay/50 rounded-lg p-2 cursor-crosshair">
    <svg 
      ref="svgRef"
      viewBox="0 0 300 150" 
      preserveAspectRatio="none"
      class="w-full aspect-2/1" 
      aria-hidden="true"
      @click="handleClick"
      @pointermove="handlePointerMove"
      @pointerleave="isHovering = false"
    >
      <!-- Grid -->
      <line x1="0" y1="150" x2="300" y2="150" class="stroke-border-default" stroke-width="0.5" />
      <line x1="0" y1="75" x2="300" y2="75" class="stroke-border-default" stroke-width="0.5" stroke-dasharray="4" />
      <line x1="0" y1="0" x2="300" y2="0" class="stroke-border-default" stroke-width="0.5" />

      <!-- Curve -->
      <polyline :points="data.polyline" fill="none" class="stroke-accent-success" stroke-width="2" />

      <!-- Current temp indicator -->
      <line
        :x1="data.currentX"
        y1="0"
        :x2="data.currentX"
        y2="150"
        class="stroke-accent-warning"
        stroke-width="1.5"
        stroke-dasharray="4"
      />
      <circle :cx="data.currentX" :cy="data.currentY" r="4" class="fill-accent-warning" />

      <!-- Hover indicator -->
      <circle v-if="isHovering" :cx="hoverX" :cy="hoverY" r="3" class="fill-accent-info opacity-50" />

      <!-- Labels -->
      <text x="5" y="145" class="fill-text-muted" font-size="10">{{ data.minT }}°</text>
      <text x="260" y="145" class="fill-text-muted" font-size="10">{{ data.maxT }}°</text>
      <text x="5" y="12" class="fill-text-muted" font-size="10">{{ data.maxD }}%</text>
      <text x="270" y="12" class="fill-text-muted" font-size="8">{{ data.minD }}%</text>
    </svg>
    <div class="h-4 text-xs text-text-secondary text-center">
      <span v-if="isHovering">{{ hoverTemp.toFixed(0) }}° → {{ hoverDuty.toFixed(0) }}%</span>
      <span v-else class="text-text-muted">Кликните для добавления точки</span>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, useTemplateRef } from 'vue';

interface GraphData {
  minT: number;
  maxT: number;
  minD: number;
  maxD: number;
  polyline: string;
  currentX: number;
  currentY: number;
}

const props = defineProps<{
  data: GraphData;
}>();

const emit = defineEmits<{
  click: [temp: number, duty: number];
}>();

const svgRef = useTemplateRef<SVGSVGElement>('svgRef');
const isHovering = ref(false);
const hoverX = ref(0);
const hoverY = ref(0);

function getSvgCoords(event: PointerEvent | MouseEvent) {
  const svg = svgRef.value;
  if (!svg) return { x: 0, y: 0 };
  
  const point = svg.createSVGPoint();
  point.x = event.clientX;
  point.y = event.clientY;
  
  const ctm = svg.getScreenCTM();
  if (!ctm) return { x: 0, y: 0 };
  
  const svgPoint = point.matrixTransform(ctm.inverse());
  return { 
    x: Math.max(0, Math.min(300, svgPoint.x)), 
    y: Math.max(0, Math.min(150, svgPoint.y)) 
  };
}

function handlePointerMove(event: PointerEvent) {
  const { x, y } = getSvgCoords(event);
  isHovering.value = true;
  hoverX.value = x;
  hoverY.value = y;
}

const hoverTemp = computed(() => {
  return props.data.minT + (hoverX.value / 300) * (props.data.maxT - props.data.minT);
});

const hoverDuty = computed(() => {
  return props.data.maxD - (hoverY.value / 150) * (props.data.maxD - props.data.minD);
});

function handleClick(event: MouseEvent) {
  const { x, y } = getSvgCoords(event);
  
  const temp = props.data.minT + (x / 300) * (props.data.maxT - props.data.minT);
  const duty = props.data.maxD - (y / 150) * (props.data.maxD - props.data.minD);
  
  emit('click', Math.round(temp), Math.round(duty));
}
</script>
