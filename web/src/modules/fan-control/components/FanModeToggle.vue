<template>
  <div class="flex gap-1">
    <button
      :class="manual ? 'bg-interactive-default text-text-primary' : 'bg-accent-success text-surface-base'"
      class="flex-1 px-3 py-1 rounded-l-md text-xs transition-all hover:brightness-110 flex items-center justify-center gap-1"
      @click="$emit('change', false)"
    >
      <Sparkles class="size-4" aria-hidden="true" />
      <span>Авто</span>
    </button>
    <button
      :class="manual ? 'bg-accent-warning text-surface-base' : 'bg-interactive-default text-text-primary'"
      class="flex-1 px-3 py-1 rounded-r-md text-xs transition-all hover:brightness-110 flex items-center justify-center gap-1"
      @click="$emit('change', true)"
    >
      <Hand class="size-4" aria-hidden="true" />
      <span>Ручной</span>
    </button>
  </div>
</template>

<script setup lang="ts">
import { Hand, Sparkles } from 'lucide-vue-next';

defineProps<{
  manual: boolean;
}>();

defineEmits<{
  change: [manual: boolean];
}>();
</script>
