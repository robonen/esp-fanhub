import { useFetch } from '@vueuse/core';
import type { Status, CurvePointApi } from './types';

export async function apiPatch(
  endpoint: string,
  params: Record<string, string | number>
): Promise<Response> {
  const body = Object.entries(params)
    .map(([k, v]) => `${encodeURIComponent(k)}=${encodeURIComponent(v)}`)
    .join('&');

  return fetch(endpoint, {
    method: 'PATCH',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body,
  });
}

export function useStatusApi() {
  return useFetch<Status>('/api/status').get().json<Status>();
}

export function useCurvePointsApi() {
  return useFetch<CurvePointApi[][]>('/api/curvePoints', { immediate: false })
    .get()
    .json<CurvePointApi[][]>();
}
