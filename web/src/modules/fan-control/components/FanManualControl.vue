<template>
  <div class="space-y-2">
    <label class="block text-xs text-text-secondary">
      Скорость: {{ modelValue }}%
      <input
        type="range"
        min="30"
        max="100"
        step="1"
        :value="modelValue"
        class="w-full mt-1 accent-accent-success"
        @input="onInput"
        @change="$emit('save')"
      />
    </label>
  </div>
</template>

<script setup lang="ts">
defineProps<{
  modelValue: number;
}>();

const emit = defineEmits<{
  'update:modelValue': [value: number];
  save: [];
}>();

function onInput(event: Event) {
  const target = event.target as HTMLInputElement;
  emit('update:modelValue', Number(target.value));
}
</script>
