#version 430 core
out vec4 fColor;

in vec3 vertexPos;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;



int id;

struct Ray{
	vec3 origin;
	vec3 direction;
};

struct hitObject{
	vec3 pos;
	vec4 color;
	float radius;
	float shinyness;
	float dist;
	bool hit;
};

struct intersectResult{
	vec3 pos;
	float dist;
	bool hit;
	vec4 color;
	float shinyness;
};

vec4 backGroundColor = vec4(0.0, 0.0 ,0.0,1.0);

Ray genRay(vec3 vertexPos);

struct Sphere
{
	vec3 pos;
	vec4 color;
	float radius;
	float shinyness;
	
};

Sphere objects[2];

struct LightSrc{
	vec3 pos;
	vec3 direction;
	vec4 color;
	float strength;
};

LightSrc light;

float facing(vec3 a, vec3 b){
	if(dot(a,b) > 0.0f)
	{
		return 1.0f;
	}
	return 0.0f;
};

vec4 shade(intersectResult result, Ray ray){
		
		vec3 n = normalize(ray.direction);

		//intersection point
		vec3 intersect = ray.origin + (result.dist)*n;

		//get surface normal
		vec3 surfaceNormal = normalize(intersect - result.pos);

//		if(result.shinyness > 40.0f)
//		{
//		//if shinyness store new direction in return objects start and return 
//			vec3 newRayDir = (-ray.direction)+2*(dot(ray.direction,surfaceNormal))*surfaceNormal;
//			rtn_ray.start = vec4(intersect+(newRayDir*0.0001),1.0);
//			rtn_ray.end = vec4(intersect+newRayDir,1.0);
//		}


		//SHADING Phong
			
		light.direction = normalize(light.pos - intersect);
		vec3 viewDir = normalize(vertexPos - intersect);
		vec3 midDir = normalize(light.direction + viewDir);


		//phong shading model // needs to be updated with pbr
			
		float ambientStr = 0.1f;

		vec4 ambient = vec4(ambientStr);
		vec4 diffuse = vec4(max(dot(surfaceNormal, light.direction),0.0f));

		vec4 specular = vec4(facing(surfaceNormal, light.direction) * pow(max(dot(surfaceNormal, midDir), 0.0f), result.shinyness));

		vec4 outColor = ambient + diffuse + specular; // reflectivity

		outColor = outColor * light.color * result.color * 1.0f;
		
		return outColor;
};


intersectResult intersect(Ray ray)
{
	intersectResult rtn;
	rtn.dist = -1;
	rtn.hit = false;

	for(int i = 0; i < objects.length(); i++){
		vec3 n = normalize(ray.direction);
		vec3 pa = objects[i].pos - ray.origin;
		float dist = length(pa);
		float a = dot(pa,n);

		//its inside the sphere, draw blue sphere
		if(dist <= objects[i].radius)
		{
			rtn.color = vec4(0.0, 0.0,1.0,1.0);
		   
			//goto next object
			rtn.hit = false;
			return rtn;
		}	

		//object is behind camera, dont draw at all
		if(dot(pa,n) < 0)
		{	
			rtn.color = vec4(1.0, 0.0,1.0,1.0);
			//go to next object
			rtn.hit = false;
			return rtn;
		}

		vec3 dVec = pa - (a*n);
		dist = length(dVec);

		if(dist <= objects[i].radius)
		{
			float x = sqrt(pow(objects[i].radius,2)-pow(dist,2));
			float hitDist = a-x;
				if(hitDist < rtn.dist || rtn.dist < 0 && hitDist != rtn.dist)
				{		
					rtn.hit = true;
					rtn.dist = hitDist;
					rtn.color = objects[i].color;
					rtn.pos = objects[i].pos;
					rtn.shinyness = objects[i].shinyness;
				}
			
		}
	}

return rtn;
}

void addObject(vec3 P, float R, vec4 C, float shinyness){
	
	objects[id].pos = P;
	objects[id].radius = R;
	objects[id].color = C;
	objects[id].shinyness = shinyness;

	id++;
}

vec4 Tracer()
{
	vec4 color;
	Ray ray = genRay(vertexPos);
	intersectResult result = intersect(ray);
	if(result.hit)
	{
		color = shade(result, ray);
	}

	//TODO intersect 
	//TODO shade
	//TOOD loop for reflection

	return color;
};

Ray genRay(vec3 vertexPos){

	Ray ray;

	vec4 startPoint = vec4(vertexPos.x,vertexPos.y,-1,1);
	vec4 endPoint = vec4(vertexPos.x,vertexPos.y,1,1);

	startPoint = projectionMatrix * startPoint;
	startPoint = viewMatrix * startPoint;
	startPoint = startPoint / startPoint.w;
	
	endPoint = projectionMatrix * endPoint;
	endPoint = viewMatrix * endPoint;
	endPoint = endPoint / endPoint.w;

	ray.origin = vec3(startPoint);
	ray.direction = vec3(normalize(endPoint - startPoint));

	//test

	return ray;

//	ray.n = normalize(ray.direction);
//	
//	//put this into its own function
//	for (int i = 0; i < objects.length(); i++)
//	{		
//		rtn = rayIntersect(objects[i], ray);
//
//		if(rtn.hit == true){
//			if(rtn.dist < hitObj.dist || hitObj.dist < 0 && rtn.dist != hitObj.dist)
//			{		
//				hitObj.pos = rtn.pos;
//				hitObj.radius = rtn.radius;
//				hitObj.color = rtn.color;
//				hitObj.dist = rtn.dist;
//				hitObj.shinyness = rtn.shinyness;	
//				hitObj.hit = true;
//			}
//		}
//	}
//
//	if(hitObj.hit == true){	
//		//if shiny spawn new ray


//
//		rtn_ray.hit = true;
//
//		//intersection point
//		vec3 intersect = ray.origin + (hitObj.dist)*ray.n;
//
//		//get surface normal
//		vec3 surfaceNormal = normalize(intersect - hitObj.pos);
//
//		if(hitObj.shinyness > 40.0f)
//		{
//		//if shinyness store new direction in return objects start and return 
//			vec3 newRayDir = (-ray.direction)+2*(dot(ray.direction,surfaceNormal))*surfaceNormal;
//			rtn_ray.start = vec4(intersect+(newRayDir*0.0001),1.0);
//			rtn_ray.end = vec4(intersect+newRayDir,1.0);
//		}
//
//		//SHADING Phong
//			
//		light.direction = normalize(light.pos - intersect);
//		vec3 viewDir = normalize(vertexPos - intersect);
//		vec3 midDir = normalize(light.direction + viewDir);
//
//
//		//phong shading model // needs to be updated with pbr
//			
//		float ambientStr = 0.1f;
//
//		vec4 ambient = vec4(ambientStr);
//		vec4 diffuse = vec4(max(dot(surfaceNormal, light.direction),0.0f));
//
//		vec4 specular = vec4(facing(surfaceNormal, light.direction) * pow(max(dot(surfaceNormal, midDir), 0.0f), hitObj.shinyness));
//
//		vec4 outColor = ambient + diffuse + specular; // reflectivity
//
//		outColor = outColor * light.color * hitObj.color * 1.0f;
//		
//		rtn_ray.color = outColor / 3.0f;
//		rtn_ray.shinyness = hitObj.shinyness;
//		
//	}
//	if(hitObj.hit == false){
//		rtn_ray.color = backGroundColor;
//		rtn_ray.hit = false;
//	}
//
//	return rtn_ray;
};


//overloaded funciton to gen ray with passing in the object it was spawned from. 

//dont intersect test spawned object

void main(){

	//generate ray from screen coords

	//set up scene
	addObject(vec3(0.0,0.0, -1.0),0.1f,vec4(0.0,1.0,0.0,1.0),50.0f);
	addObject(vec3(0.3,0.0, -2.0),0.1f,vec4(0.0,1.0,0.0,1.0),10.0f);

	light.pos = vec3(-10.0,1.0,10.0);
	light.color = vec4(1.0);

	//if hit object is shiny then gen new ray and i--; else fColor = hitObject color;

//	start = vec4(vertexPos.x,vertexPos.y,-1,1);
//	end = vec4(vertexPos.x,vertexPos.y,1,1);

	//make tracer funcitons.
	//ray gen for ncd
	//ray gen for reflections which are in world space
	//intersect and closest obj
	//shade
	//loop for reflections
	
//		rtn_ray = genRay(vertexPos);

		fColor = Tracer();

//	for(int i = 0; i < 1; i++){
//	//genRay must return a rayObject;
//
//		if(rtn_ray.hit == true && rtn_ray.shinyness > 40.0f)
//		{
//			fColor = rtn_ray.color;
//			if(bounceCounter <= 0) //just get the color of the last reflected object and then display it
//			{
//				continue;
//			}
//			bounceCounter--;
//			i--;
//			start = rtn_ray.start;
//			end = rtn_ray.end;
//		//rtn. intersect point, and ray direction are required to find start point and direction, add direction /10 to start as a offset to ensure the start doest
//		//collide with the original object, add the whole direction to start to get the end point and pass it back into the ray
//			continue;
//		}else if(rtn_ray.hit == true){
//			fColor = rtn_ray.color;
//			continue;
//		}
//		//if it doesnt hit or hits and its a normal object, distplay the color
//		fColor = rtn_ray.color;
//	}
	
	return;
}