'use strict';
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
const {trajectory}=require('../tools/compare.cjs');
const [dir,mode]=process.argv.slice(2),json=JSON.parse(fs.readFileSync(path.join(dir,'result.json'),'utf8'));
const rows=trajectory(fs.readFileSync(path.join(dir,'trajectory.csv'),'utf8')).rows.filter(r=>r[2]===1);
const collision=json.events.find(e=>e.kind===1);assert(collision,'a real first contact is required');
const after=rows.filter(r=>r[0]>collision.step),radius=json.initial.balls[0].radius;
if(mode==='follow')assert(after.some(r=>r[6]>0.01),'follow must advance after collision');
if(mode==='draw')assert(after.some(r=>r[6]<-0.01),'draw must reverse after collision');
let transition=-1,hadSlip=false;
for(const r of rows) {
  const slip=Math.hypot(r[6]-r[10]*radius,r[7]+r[9]*radius);
  if(slip>1e-6)hadSlip=true;
  if(hadSlip && Math.hypot(r[6],r[7])>0.001 && slip<1e-8){transition=r[0];break;}
}
assert(transition>=0,'moving slide-to-roll transition must be recorded');
assert.equal(rows.at(-1)[12],1);assert.deepEqual(rows.at(-1).slice(6,12),[0,0,0,0,0,0]);
console.log(JSON.stringify({mode,contactStep:collision.step,rollingStep:transition,stopStep:json.final.step,stopTime:json.final.time}));
