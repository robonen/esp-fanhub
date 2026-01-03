import { computed, type ComputedRef } from 'vue';
import type { CurvePoint } from '../../../common/types';
import { useFanController } from './useFanController';

interface GraphBounds {
  minT: number;
  maxT: number;
  minD: number;
  maxD: number;
}

export interface GraphData extends GraphBounds {
  polyline: string;
  currentX: number;
  currentY: number;
}

function getGraphBounds(pts: readonly CurvePoint[], currentTemp: number): GraphBounds {
  const temps = [...pts.map((p) => p.temp), currentTemp];
  const duties = pts.map((p) => p.duty);

  const minT = Math.floor(Math.min(...temps) / 5) * 5 - 5;
  const maxT = Math.ceil(Math.max(...temps) / 5) * 5 + 5;
  const minD = Math.floor(Math.min(...duties, 30) / 10) * 10;
  const maxD = Math.ceil(Math.max(...duties, 100) / 10) * 10;

  return {
    minT: Math.max(0, minT),
    maxT: Math.min(120, maxT),
    minD: Math.max(0, minD),
    maxD: Math.min(100, maxD),
  };
}

function tempToX(t: number, minT: number, maxT: number): number {
  return ((t - minT) / (maxT - minT)) * 300;
}

function dutyToY(d: number, minD: number, maxD: number): number {
  return 150 - ((d - minD) / (maxD - minD)) * 150;
}

function polylineFor(
  pts: readonly CurvePoint[],
  minT: number,
  maxT: number,
  minD: number,
  maxD: number
): string {
  return [...pts]
    .sort((a, b) => a.temp - b.temp)
    .map((pt) => `${tempToX(pt.temp, minT, maxT)},${dutyToY(pt.duty, minD, maxD)}`)
    .join(' ');
}

export function useFanGraph(fanIndex: number): ComputedRef<GraphData> {
  const { points, fans, tempC } = useFanController();
  
  return computed(() => {
    const pts = points.value[fanIndex];
    const bounds = getGraphBounds(pts, tempC.value);
    const fanDuty = fans.value[fanIndex]?.dutyPct ?? 30;

    return {
      ...bounds,
      polyline: polylineFor(pts, bounds.minT, bounds.maxT, bounds.minD, bounds.maxD),
      currentX: tempToX(tempC.value, bounds.minT, bounds.maxT),
      currentY: dutyToY(fanDuty, bounds.minD, bounds.maxD),
    };
  });
}
