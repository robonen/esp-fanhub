<template>
  <button
    ref="btnRef"
    class="px-3 py-2 rounded-md border border-border-default bg-interactive-default hover:bg-interactive-hover transition inline-flex items-center justify-center"
    type="button"
    :aria-label="isDark ? 'Switch to light theme' : 'Switch to dark theme'"
    @click="toggleWithTransition"
  >
    <Moon v-if="isDark" class="size-4" aria-hidden="true" />
    <Sun v-else class="size-4" aria-hidden="true" />
  </button>
</template>

<script setup lang="ts">
import { useTemplateRef } from 'vue';
import { useDark, useToggle } from '@vueuse/core';
import { Moon, Sun } from 'lucide-vue-next';

const btnRef = useTemplateRef<HTMLButtonElement>('btnRef');

const isDark = useDark({
  selector: 'html',
  attribute: 'data-theme',
  valueDark: 'dark',
  valueLight: 'light',
});

const toggle = useToggle(isDark);

function toggleWithTransition() {
  if (!document.startViewTransition) {
    toggle();
    return;
  }

  const switchingToDark = !isDark.value;

  const btn = btnRef.value;
  const x = btn ? btn.offsetLeft + btn.offsetWidth / 2 : window.innerWidth / 2;
  const y = btn ? btn.offsetTop + btn.offsetHeight / 2 : 0;

  const endRadius = Math.hypot(
    Math.max(x, window.innerWidth - x),
    Math.max(y, window.innerHeight - y)
  );

  if (!switchingToDark) {
    document.documentElement.dataset.themeSwitch = 'true';
  }

  const transition = document.startViewTransition(() => {
    toggle();
  });

  transition.ready.then(() => {
    const clipPath = [
      `circle(0px at ${x}px ${y}px)`,
      `circle(${endRadius}px at ${x}px ${y}px)`,
    ];

    document.documentElement.animate(
      { clipPath: switchingToDark ? clipPath : [...clipPath].reverse() },
      {
        duration: 350,
        easing: 'ease-out',
        fill: 'forwards',
        pseudoElement: switchingToDark
          ? '::view-transition-new(root)'
          : '::view-transition-old(root)',
      }
    );
  });

  transition.finished.then(() => {
    delete document.documentElement.dataset.themeSwitch;
  });
}
</script>

<style>
::view-transition-old(root),
::view-transition-new(root) {
  animation: none;
  mix-blend-mode: normal;
}

::view-transition-old(root) {
  z-index: 1;
}

::view-transition-new(root) {
  z-index: 999;
}

[data-theme-switch]::view-transition-old(root) {
  z-index: 999;
}

[data-theme-switch]::view-transition-new(root) {
  z-index: 1;
}
</style>
