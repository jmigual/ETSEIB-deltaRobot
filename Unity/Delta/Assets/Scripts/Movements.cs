using UnityEngine;
using System.Collections;
using System.IO;
using System;

public class Movements : MonoBehaviour {

	public float height=10f;
	public float radius=20f;
	public float w=3f;
	public RobotCreator robot;
	private float t=0f;
	public string path;

	public float rangexz, rangey, steps;

	void Start(){

		if (robot.calculateWorkspace){
			
			string content="";
			Transform trans = robot.GetComponentInParent<Transform>();
			
			for (float dy=0; dy<rangey; dy+=rangey/steps){
				for (float dx=-rangexz/2; dx<=rangexz/2; dx+=rangexz/steps){
					for (float dz=-rangexz/2; dz<=rangexz/2; dz+=rangexz/steps){
						Vector3 d= new Vector3 (dx,-dy, dz);
						robot.Ocontrolled=trans.position+d;
						robot.setAngles();
						if (robot.valid){
							Vector3 res=robot.Ocontrolled-trans.position;
							content = content + res.x.ToString("F2") +" " +
												res.z.ToString("F2") + " " + 
												res.y.ToString("F2") + Environment.NewLine;
						}
					}
				}
			}
			
			System.IO.File.WriteAllText(path, content);
		}
	}

	void Update () {
		t+=Time.deltaTime;
		if ((w*t)>(2*Mathf.PI))t-=2*Mathf.PI/w;
		robot.Ocontrolled.Set (radius*Mathf.Sin (w*t),height,radius*Mathf.Cos (w*t));
		//Debug.Log (robot.valid);
	}
}
