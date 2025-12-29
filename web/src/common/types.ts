export interface FanStatus {
  index: number;
  dutyPct: number;
  manualDuty: number;
  manual: boolean;
}

export interface Status {
  wifi: boolean;
  ip: string;
  tempC: number;
  fans: FanStatus[];
}

export interface CurvePoint {
  temp: number;
  duty: number;
}

export interface CurvePointApi {
  t: number;
  d: number;
}
