<template>
  <div class="flex gap-1">
    <button
      :class="manual ? 'bg-slate-700' : 'bg-emerald-700'"
      class="flex-1 px-2 py-1 rounded-l-md text-xs hover:brightness-110"
      @click="$emit('change', false)"
    >
      Авто
    </button>
    <button
      :class="manual ? 'bg-amber-700' : 'bg-slate-700'"
      class="flex-1 px-2 py-1 rounded-r-md text-xs hover:brightness-110"
      @click="$emit('change', true)"
    >
      Ручной
    </button>
  </div>
</template>

<script setup lang="ts">
defineProps<{
  manual: boolean;
}>();

defineEmits<{
  change: [manual: boolean];
}>();
</script>
