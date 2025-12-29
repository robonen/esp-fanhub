import { computed, ref, watch } from 'vue';
import { useIntervalFn } from '@vueuse/core';
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

export function useFanController() {
  const { data: statusData, execute: refreshStatus } = useStatusApi();
  const { data: pointsData, execute: refreshPoints } = useCurvePointsApi();

  const manualDuty = ref([50, 50, 50]);
  const points = ref<CurvePoint[][]>(structuredClone(DEFAULT_POINTS));
  const isEditing = ref(false);

  const wifi = computed(() => statusData.value?.wifi ?? false);
  const ip = computed(() => statusData.value?.ip ?? '');
  const tempC = computed(() => statusData.value?.tempC ?? 0);
  const fans = computed(() => statusData.value?.fans ?? DEFAULT_FANS);

  watch(statusData, (data) => {
    if (data?.fans) {
      data.fans.forEach((f, i) => {
        manualDuty.value[i] = f.manualDuty;
      });
    }
  });

  watch(pointsData, (data) => {
    if (data && !isEditing.value) {
      points.value = data.map((fan) =>
        fan.map((p) => ({ temp: p.t, duty: p.d }))
      );
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
    await refreshStatus();
  }

  function addPoint(fanIndex: number): void {
    points.value[fanIndex].push({
      temp: Math.round(tempC.value),
      duty: Math.round(fans.value[fanIndex]?.dutyPct ?? 60),
    });
  }

  function removePoint(fanIndex: number, pointIndex: number): void {
    if (points.value[fanIndex].length > 1) {
      points.value[fanIndex].splice(pointIndex, 1);
    }
  }

  refreshPoints();
  useIntervalFn(refreshStatus, 1500);

  return {
    wifi,
    ip,
    tempC,
    fans,
    manualDuty,
    points,
    isEditing,
    setMode,
    setManualDuty,
    savePoints,
    addPoint,
    removePoint,
  };
}
