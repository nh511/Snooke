'use strict';
const assert=require('node:assert/strict'),fs=require('node:fs'),path=require('node:path');
const dir=process.argv[2];const text=fs.readFileSync(path.join(dir,'performance.csv'),'utf8').trim().split(/\r?\n/);
assert.equal(text.shift(),'sample_kind,step,time_s,elapsed_ns,search_ns,ball_ball_ns,cushion_ns,jaw_ns,pocket_contact_ns,mouth_entered_ns,recovered_to_cloth_ns,total_events,ball_ball,cushion,jaw,pocket_contact,mouth_entered,potted,recovered_to_cloth,ball_settled,shot_settled');
assert(text.length>2);const rows=text.map(line=>line.split(','));const total=rows.at(-1);assert.equal(total[0],'shot');
for(const row of rows){assert.equal(row.length,21);for(let i=1;i<row.length;i++)assert(Number.isFinite(Number(row[i])));assert(Number(row[3])>=0);}
assert(Number(total[1])>0);assert(Number(total[4])>0);assert(Number(total[5])>0);assert(Number(total[12])>0);
const summary=fs.readFileSync(path.join(dir,'performance-summary.txt'),'utf8');
for(const field of ['schema=snooker-performance-v1','search_time_ns=','ball_ball_time_ns=','cushion_time_ns=','pocket_contact_time_ns='])assert(summary.includes(field));
console.log('PASS per-step and per-shot performance CSV');
