(function(root){
  'use strict';
  const header='step,time_s,id,x_m,y_m,z_m,vx_mps,vy_mps,vz_mps,wx_radps,wy_radps,wz_radps,sleeping,pocket_state,pocket_id,pot_emitted,event_count';
  function parse(json,csv){
    if(typeof csv!=='string'||csv.length>64*1024*1024||typeof json!=='string'||json.length>16*1024*1024)throw Error('File size limit exceeded');
    const result=JSON.parse(json),frames=[];
    if(result.schema!=='snooker-result-v1'||result.result!=='Settled'||!Array.isArray(result.final?.balls)||!Array.isArray(result.events))throw Error('Unsupported result');
    const ids=result.final.balls.map(b=>b.id);
    if(!ids.length||ids.length>64||new Set(ids).size!==ids.length||ids.some(id=>!Number.isSafeInteger(id)||id<=0))throw Error('Invalid ball IDs');
    const lines=csv.trim().split(/\r?\n/);if(lines.shift()!==header||!lines.length||lines.length>1000000)throw Error('Invalid trajectory schema/count');
    for(const line of lines){
      const v=line.split(',').map(Number);
      if(v.length!==17||!v.every(Number.isFinite)||!Number.isSafeInteger(v[0])||v[0]<0||v[1]<0||!ids.includes(v[2])||![0,1].includes(v[12])||!Number.isInteger(v[13])||v[13]<0||v[13]>4||![0,1].includes(v[15]))throw Error('Invalid trajectory values');
      let frame=frames.at(-1);
      if(!frame||frame.step!==v[0]){
        if(frame&&(v[0]!==frame.step+1||v[1]<=frame.time||frame.balls.length!==ids.length))throw Error('Missing/unordered physics frame');
        frame={step:v[0],time:v[1],balls:[]};frames.push(frame);
      }
      if(frame.time!==v[1]||v[2]!==ids[frame.balls.length])throw Error('Duplicate/unordered ball rows');
      frame.balls.push({id:v[2],position:v.slice(3,6),velocity:v.slice(6,9),angular:v.slice(9,12),sleeping:!!v[12],pocketState:v[13],pocketId:v[14],potEmitted:!!v[15]});
    }
    if(frames.at(-1).balls.length!==ids.length||frames[0].step!==result.initial.step||frames.at(-1).step!==result.final.step)throw Error('Result/trajectory range mismatch');
    for(let i=0;i<ids.length;i++){
      const a=frames.at(-1).balls[i],b=result.final.balls[i];
      if(JSON.stringify(a.position)!==JSON.stringify(b.position)||JSON.stringify(a.velocity)!==JSON.stringify(b.velocity)||a.pocketState!==b.pocketState)throw Error('Final state mismatch');
    }
    let time=frames[0].time;
    for(const e of result.events){if(!Number.isFinite(e.time)||e.time<time||e.time>frames.at(-1).time+1e-12||!Number.isInteger(e.step)||e.step<frames[0].step||e.step>frames.at(-1).step||!Array.isArray(e.point)||e.point.length!==3||!e.point.every(Number.isFinite))throw Error('Invalid event');time=e.time;}
    if(!Array.isArray(result.geometry?.segments)||!result.geometry.segments.length||!Array.isArray(result.geometry?.pockets))throw Error('Missing geometry');
    for(const s of result.geometry.segments)for(const p of [s.start,s.end])if(!Array.isArray(p)||p.length!==3||!p.every(Number.isFinite))throw Error('Invalid contour');
    for(const p of result.geometry.pockets)if(!Array.isArray(p.center)||!p.center.every(Number.isFinite)||!Number.isFinite(p.opening)||p.opening<=0)throw Error('Invalid pocket geometry');
    return {result,frames};
  }
  if(typeof module!=='undefined')module.exports={parse};else root.SnookerShotData={parse};
})(typeof globalThis!=='undefined'?globalThis:this);
