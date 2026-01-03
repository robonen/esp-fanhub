import { computed, ref, watch } from 'vue';
import { createGlobalState, useIntervalFn } from '@vueuse/core';
import { useStatusApi, useCurvePointsApi, apiPatch } from '../../../common/api';
import type { FanStatus, CurvePoint } from '../../../common/types';

const DEFAULT_FANS: FanStatus[] = [
  { index: 0, dutyPct: 30, manualDuty: 50, manual: false },
  { index: 1, dutyPct: 30, manualDuty: 50, manual: false },
  { index: 2, dutyPct: 30, manualDuty: 50, manual: false },
];

const DEFAULT_POINTS: CurvePoint[][] = [
  [{ temp: 35, duty: 35 }, { temp: 75, duty: 100 }],
  [{ temp: 35, duty: 35 }, { temp: 75, duty: 100 }],
  [{ temp: 35, duty: 35 }, { temp: 75, duty: 100 }],
];

function pointsEqual(a: CurvePoint[], b: CurvePoint[]): boolean {
  if (a.length !== b.length) return false;
  const sortedA = [...a].sort((x, y) => x.temp - y.temp);
  const sortedB = [...b].sort((x, y) => x.temp - y.temp);
  return sortedA.every((p, i) => p.temp === sortedB[i].temp && p.duty === sortedB[i].duty);
}

export const useFanController = createGlobalState(() => {
  const { data: statusData, execute: refreshStatus } = useStatusApi();
  const { data: pointsData, execute: refreshPoints } = useCurvePointsApi();

  const activeFan = ref(0);
  const manualDuty = ref([50, 50, 50]);
  const points = ref<CurvePoint[][]>(structuredClone(DEFAULT_POINTS));
  const savedPoints = ref<CurvePoint[][]>(structuredClone(DEFAULT_POINTS));
  const isEditing = ref(false);

  const wifi = computed(() => statusData.value?.wifi ?? false);
  const ip = computed(() => statusData.value?.ip ?? '');
  const tempC = computed(() => statusData.value?.tempC ?? 0);
  const fans = computed(() => statusData.value?.fans ?? DEFAULT_FANS);

  const hasUnsavedChanges = computed(() => {
    return points.value.map((pts, i) => !pointsEqual(pts, savedPoints.value[i]));
  });

  watch(statusData, (data) => {
    if (data?.fans) {
      data.fans.forEach((f, i) => {
        manualDuty.value[i] = f.manualDuty;
      });
    }
  });

  watch(pointsData, (data) => {
    if (data && !isEditing.value) {
      const parsed = data.map((fan) =>
        fan.map((p) => ({ temp: p.t, duty: p.d }))
      );
      points.value = structuredClone(parsed);
      savedPoints.value = structuredClone(parsed);
    }
  });

  async function setMode(fanIndex: number, manual: boolean): Promise<void> {
    await apiPatch('/api/mode', { id: fanIndex, manual: manual ? 1 : 0 });
    await refreshStatus();
  }

  async function setManualDuty(fanIndex: number, duty: number): Promise<void> {
    await apiPatch('/api/duty', { id: fanIndex, duty });
    await refreshStatus();
  }

  async function savePoints(fanIndex: number): Promise<void> {
    const csv = [...points.value[fanIndex]]
      .sort((a, b) => a.temp - b.temp)
      .map((p) => `${p.temp}:${p.duty}`)
      .join(',');
    isEditing.value = false;
    await apiPatch('/api/curvePoints', { id: fanIndex, points: csv });
    savedPoints.value[fanIndex] = structuredClone(points.value[fanIndex]);
    await refreshStatus();
  }

  function addPoint(fanIndex: number, temp?: number, duty?: number): void {
    points.value[fanIndex].push({
      temp: temp ?? Math.round(tempC.value),
      duty: duty ?? Math.round(fans.value[fanIndex]?.dutyPct ?? 60),
    });
  }

  function removePoint(fanIndex: number, pointIndex: number): void {
    if (points.value[fanIndex].length > 1) {
      points.value[fanIndex].splice(pointIndex, 1);
    }
  }

  function updatePoints(fanIndex: number, newPoints: CurvePoint[]): void {
    points.value[fanIndex] = newPoints;
  }

  refreshPoints();
  useIntervalFn(refreshStatus, 1500);

  return {
    wifi,
    ip,
    tempC,
    fans,
    activeFan,
    manualDuty,
    points,
    isEditing,
    hasUnsavedChanges,
    setMode,
    setManualDuty,
    savePoints,
    addPoint,
    removePoint,
    updatePoints,
  };
});
