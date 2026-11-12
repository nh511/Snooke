(function (root) {
  'use strict';
  const header = 'step,time_s,id,x_m,y_m,z_m,vx_mps,vy_mps,vz_mps,wx_radps,wy_radps,wz_radps,sleeping,table_length_m,table_width_m,radius_m,parameter_version';
  function parse(text) {
    if (typeof text !== 'string' || text.length > 32 * 1024 * 1024) throw new Error('CSV exceeds 32 MiB');
    const lines = text.trim().split(/\r?\n/);
    if (lines.shift() !== header || !lines.length || lines.length > 300000) throw new Error('Invalid CSV header or row count');
    const rows = lines.map((line) => {
      const cells = line.split(',');
      if (cells.length !== 17 || cells.slice(0,16).some(v => !v.trim())) throw new Error('Invalid CSV fields');
      const n = cells.slice(0,16).map(Number);
      if (n.some(v => !Number.isFinite(v)) || !/^[A-Za-z0-9_.-]+$/.test(cells[16])) throw new Error('Invalid value or version token');
      const [step,time,id,x,y,z,vx,vy,vz,wx,wy,wz,sleeping,length,width,radius] = n;
      if (!Number.isSafeInteger(step) || step < 0 || time < 0 || !Number.isSafeInteger(id) || id <= 0 ||
          ![0,1].includes(sleeping) || length<=0 || width<=0 || radius<=0 || radius*2>=Math.min(length,width)) throw new Error('Out-of-range CSV state');
      return {step,time,id,x,y,z,vx,vy,vz,wx,wy,wz,sleeping,length,width,radius,version:cells[16]};
    });
    for (let i=1;i<rows.length;i++) {
      const a=rows[i-1], b=rows[i];
      if (b.step<=a.step || b.time<=a.time || b.id!==a.id || b.version!==a.version || b.length!==a.length || b.width!==a.width || b.radius!==a.radius)
        throw new Error('CSV must be one ordered single-ball trajectory with constant configuration');
    }
    return rows;
  }
  if (typeof module !== 'undefined' && module.exports) module.exports = {parse,header};
  else root.SnookerTrajectory = {parse,header};
})(typeof globalThis !== 'undefined' ? globalThis : this);
