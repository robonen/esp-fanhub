<template>
  <div class="min-h-screen p-4">
    <div class="max-w-xl mx-auto">
      <!-- Header -->
      <div class="p-4 rounded-xl border border-border-muted bg-surface-elevated/60 mb-4 space-y-2">
        <div class="flex items-center justify-between gap-3">
          <h1 class="text-xl font-semibold">ESP FanHub</h1>
          <ThemeToggle />
        </div>
        <div class="flex items-center gap-4">
          <p class="text-sm">
            <span class="text-text-secondary">Wi-Fi:</span>
            <span :class="wifi ? 'text-accent-success' : 'text-accent-danger'" class="ml-1">
              {{ wifi ? 'Подключено' : 'Подключение...' }}
            </span>
            <span v-if="wifi && ip" class="text-text-muted ml-1">({{ ip }})</span>
          </p>
          <p class="text-sm">
            <span class="text-text-secondary">Температура CPU:</span>
            <span class="font-mono text-accent-warning ml-1">{{ tempC.toFixed(1) }}°C</span>
          </p>
        </div>
      </div>

      <!-- Fan Tabs -->
      <div class="flex gap-1 mb-4">
        <button
          v-for="(fan, i) in fans"
          :key="i"
          :class="[
            'flex-1 px-3 py-2 rounded-lg text-sm font-medium transition',
            activeFan === i 
              ? 'bg-surface-elevated border border-border-default text-text-primary' 
              : 'bg-surface-overlay/50 text-text-secondary hover:bg-surface-overlay'
          ]"
          @click="activeFan = i"
        >
          <div class="flex items-center justify-center gap-2">
            <span>Вент {{ i + 1 }}</span>
            <span 
              :class="[
                'text-xs px-1.5 py-0.5 rounded',
                fan.manual ? 'bg-accent-warning/20 text-accent-warning' : 'bg-accent-success/20 text-accent-success'
              ]"
            >
              {{ fan.dutyPct.toFixed(0) }}%
            </span>
            <span 
              v-if="hasUnsavedChanges[i] && !fan.manual" 
              class="size-2 rounded-full bg-accent-info"
            />
          </div>
        </button>
      </div>

      <!-- Active Fan Card -->
      <FanCard :fan-index="activeFan" />
    </div>
  </div>
</template>

<script setup lang="ts">
import { FanCard, useFanController } from '../../modules/fan-control';
import ThemeToggle from '../../modules/ui/components/ThemeToggle.vue';

const {
  wifi,
  ip,
  tempC,
  fans,
  activeFan,
  hasUnsavedChanges,
} = useFanController();
</script>
