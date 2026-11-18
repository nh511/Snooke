'use strict';
const assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path');
const {compare,trajectory,structured,directories}=require('../tools/compare.cjs');
const trace=fs.readFileSync(path.join(__dirname,'../data/baselines/center/trajectory.csv'),'utf8');
assert.equal(compare(trace,trace).pass,true);
const lines=trace.trim().split(/\r?\n/),row=lines[4].split(',');row[3]=String(Number(row[3])+1e-6);
const changed=lines.slice();changed[4]=row.join(',');
const diff=compare(trace,changed.join('\n'),1e-9);
assert.equal(diff.pass,false);assert.equal(diff.step,3);assert.equal(diff.ball,1);assert.equal(diff.field,'x_m');
assert.equal(compare(trace,changed.join('\n'),1e-5).pass,true);
assert.throws(()=>compare(trace,trace,-1));assert.throws(()=>trajectory('invalid'));
for(const field of ['time','velocity','pocketState','sequence']) {
  const a={events:[{sequence:0,time:1,ball:1,step:240}],final:{velocity:[1,0,0],pocketState:0}};
  const b=structuredClone(a);
  if(field==='time')b.events[0].time+=1e-6;
  if(field==='sequence')b.events[0].sequence++;
  if(field==='velocity')b.final.velocity[0]+=1e-6;
  if(field==='pocketState')b.final.pocketState++;
  assert.equal(structured(a,b,0).pass,false);
}
assert.equal(structured({time:1},{time:1+1e-10},1e-9),null);
assert.equal(structured({step:1},{step:2},100).pass,false);
if(process.argv[2])assert.equal(directories(process.argv[2],process.argv[3],0).pass,true);
console.log('PASS first-divergence and exact/tolerant state/event comparison');
