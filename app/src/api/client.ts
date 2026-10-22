// HTTP client nói chuyện với firmware qua Wi-Fi AP của jammer.
// Firmware mặc định: SoftAP "jammer" -> http://192.168.4.1
// Điều khiển qua /WebCommand?command=<text> (giống lệnh serial).

const DEFAULT_BASE = 'http://192.168.4.1';
let baseUrl = DEFAULT_BASE;

export const setBaseUrl = (u: string) => {
  baseUrl = u.replace(/\/+$/, '');
};
export const getBaseUrl = () => baseUrl;

export interface DeviceStatus {
  nrf24_count: number;
  bt_channels_text: string;
  wifi_jam_method: number;
  ssids_by_channel: string[];
  wifi_channels: number[];
}

async function req(path: string, timeoutMs = 6000): Promise<Response> {
  const ctrl = new AbortController();
  const t = setTimeout(() => ctrl.abort(), timeoutMs);
  try {
    const res = await fetch(baseUrl + path, { signal: ctrl.signal });
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    return res;
  } finally {
    clearTimeout(t);
  }
}

/** Gửi 1 lệnh text tới firmware. Trả về text phản hồi. */
export async function sendCommand(cmd: string): Promise<string> {
  const res = await req('/WebCommand?command=' + encodeURIComponent(cmd));
  return res.text();
}

/** Lấy trạng thái thiết bị. Thành công = đang kết nối. */
export async function getStatus(): Promise<DeviceStatus> {
  const res = await req('/api/status');
  return res.json();
}

export async function getVersion(): Promise<string> {
  const res = await req('/api/version');
  return res.text();
}

// Tiện ích
export const stopAll = () => sendCommand('stop');
export const reboot = () => sendCommand('reboot');
export const scanWifi = () => sendCommand('scan wifi');

/** Danh sách AP quét được, kèm kênh. Lọc bỏ ô rỗng. */
export async function wifiAps(): Promise<{ ssid: string; channel: number }[]> {
  const s = await getStatus();
  const out: { ssid: string; channel: number }[] = [];
  s.ssids_by_channel.forEach((ssid, i) => {
    if (ssid && ssid.trim().length > 0) {
      out.push({ ssid, channel: s.wifi_channels[i] ?? i + 1 });
    }
  });
  return out;
}
