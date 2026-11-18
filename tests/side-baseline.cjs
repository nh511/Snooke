'use strict';
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
const {trajectory}=require('../tools/compare.cjs');
const [leftDir,rightDir]=process.argv.slice(2);
const load=(dir)=>({rows:trajectory(fs.readFileSync(path.join(dir,'trajectory.csv'),'utf8')).rows,json:JSON.parse(fs.readFileSync(path.join(dir,'result.json'),'utf8'))});
const l=load(leftDir),r=load(rightDir);assert.equal(l.rows.length,r.rows.length);
// Polar vectors reflect Y; axial angular vectors reflect X/Z.
const flip=new Set([4,7,9,11]);
for(let i=0;i<l.rows.length;i++)for(let j=0;j<17;j++) {
  if(j===14)continue; // Mirror pockets may have different IDs.
  assert(Math.abs(l.rows[i][j]-(flip.has(j)?-r.rows[i][j]:r.rows[i][j]))<=1e-9,`mirror row ${i} field ${j}`);
}
for(const run of [l,r]) {
  assert(run.json.events.some(e=>e.kind===2),'actual cushion impact required');
  assert(run.rows.some(row=>Math.abs(row[7])>1e-4),'side spin must cause nonzero lateral velocity');
  assert.equal(run.rows.at(-1)[12],1);
}
assert(l.rows[0][11]*r.rows[0][11]<0);
console.log(`PASS mirrored side-spin: ${l.rows.length} samples, stop=${l.json.final.time}`);
