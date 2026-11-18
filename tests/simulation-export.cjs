'use strict';
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
const dir=process.argv[2];
const result=JSON.parse(fs.readFileSync(path.join(dir,'result.json'),'utf8'));
assert.equal(result.schema,'snooker-result-v1');assert.equal(result.result,'Settled');
assert.equal(result.initial.balls.length,22);assert.equal(result.final.balls.length,22);
assert(result.bundle.includes(result.parameterVersion));assert.equal(result.geometry.pockets.length,6);
const events=fs.readFileSync(path.join(dir,'events.txt'),'utf8').trim().split('\n');
assert.equal(result.events.length,events.length);assert.equal(result.final.eventCount,result.events.length+result.initial.eventCount);
for(let i=0;i<events.length;i++) {
  const tokens=events[i].split(' '),e=result.events[i];
  assert.equal(e.sequence,Number(tokens[0]));assert.equal(e.step,Number(tokens[1]));assert.equal(e.time,Number(tokens[2]));
  assert.equal(e.kind,Number(tokens[3]));assert.equal(e.ball,Number(tokens[4]));assert.equal(e.other,Number(tokens[5]));
  assert.deepEqual(e.point,tokens[6].split(',').map(Number));assert.deepEqual(e.normal,tokens[7].split(',').map(Number));assert.equal(e.impulse,Number(tokens[8]));
  if(i)assert(e.time>=result.events[i-1].time);
}
const csv=fs.readFileSync(path.join(dir,'trajectory.csv'),'utf8').trim().split('\n');
assert.equal(csv.shift(),'step,time_s,id,x_m,y_m,z_m,vx_mps,vy_mps,vz_mps,wx_radps,wy_radps,wz_radps,sleeping,pocket_state,pocket_id,pot_emitted,event_count');
const ids=result.final.balls.map(b=>b.id),count=ids.length;
assert.equal(csv.length,(result.final.step-result.initial.step+1)*count);
for(let i=0;i<csv.length;i++) {
  const v=csv[i].split(',').map(Number);assert.equal(v.length,17);assert(v.every(Number.isFinite));
  assert.equal(v[0],result.initial.step+Math.floor(i/count));assert.equal(v[2],ids[i%count]);
  assert.equal(v[1],v[0]*Number(result.bundle.split(' ')[2]));
  if(i>=csv.length-count) {
    const b=result.final.balls[i%count];assert.deepEqual(v.slice(3,6),b.position);
    assert.deepEqual(v.slice(6,9),b.velocity);assert.deepEqual(v.slice(9,12),b.angular);
    assert.equal(v[12],Number(b.sleeping));assert.equal(v[13],b.pocketState);assert.equal(v[14],b.pocketId);
    assert.equal(v[15],Number(b.potEmitted));assert.equal(v[16],result.final.eventCount);
  }
}
assert.equal(fs.readFileSync(path.join(dir,'final.snapshot'),'utf8'),result.final.serialized);
assert.deepEqual(JSON.parse(fs.readFileSync(path.join(dir,'viewer.json'),'utf8')),result);
console.log(`PASS independent JSON/CSV parse: ${csv.length} rows, ${events.length} events`);
