<template>
  <button
    class="px-3 py-2 rounded-md border border-border-default bg-interactive-default hover:bg-interactive-hover transition inline-flex items-center justify-center"
    type="button"
    :aria-label="isDark ? 'Switch to light theme' : 'Switch to dark theme'"
    @click="toggle()"
  >
    <Moon v-if="isDark" class="size-4" aria-hidden="true" />
    <Sun v-else class="size-4" aria-hidden="true" />
  </button>
</template>

<script setup lang="ts">
import { useDark, useToggle } from '@vueuse/core';
import { Moon, Sun } from 'lucide-vue-next';

const isDark = useDark({
  selector: 'html',
  attribute: 'data-theme',
  valueDark: 'dark',
  valueLight: 'light',
});

const toggle = useToggle(isDark);
</script>
