<template>
  <div class="space-y-2">
    <label class="block text-xs text-text-secondary">
      Скорость: {{ manualDuty[fanIndex] }}%
      <input
        type="range"
        min="30"
        max="100"
        step="1"
        :value="manualDuty[fanIndex]"
        class="w-full mt-1 accent-accent-success"
        @input="onInput"
        @change="setManualDuty(fanIndex, manualDuty[fanIndex])"
      />
    </label>
  </div>
</template>

<script setup lang="ts">
import { useFanController } from '../composables/useFanController';

const props = defineProps<{
  fanIndex: number;
}>();

const { manualDuty, setManualDuty } = useFanController();

function onInput(event: Event) {
  const target = event.target as HTMLInputElement;
  manualDuty.value[props.fanIndex] = Number(target.value);
}
</script>
