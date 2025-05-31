#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"

using namespace cgp;




void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 15.0f,6.0f,6.0f }, {0,0,0});
	

	// General information
	display_info();

	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	int N_terrain_samples = 300, n_col = 50;
	float terrain_length = 50;

	terrain.create_terrain_mesh(N_terrain_samples, terrain_length, n_col);
	
	terrain_mesh.initialize_data_on_gpu(terrain.mesh);
	terrain_mesh.material.color = { 0.6f,0.85f,0.5f };
	terrain_mesh.material.phong.specular = 0.0f; // non-specular terrain material
	terrain_mesh.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/texture_grass.jpg", GL_REPEAT, GL_REPEAT);

	// mesh const tree_mesh = create_tree();
	// tree.initialize_data_on_gpu(tree_mesh);

	// tree_position = terrain.generate_positions_on_terrain(N_trees);

	// tree.initialize_supplementary_data_on_gpu(cgp::numarray<vec3>(tree_position), 4, 1);
}


void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	draw(terrain_mesh, environment);
	
	// draw(tree, environment, N_trees);

	if (gui.display_wireframe)
	{
		draw_wireframe(terrain_mesh, environment);
		
		// for (vec3 pos: tree_position)
		// {
		// 	tree.model.translation = pos;
		// 	tree.model.translation.z -= 0.1f;
		// 	draw_wireframe(tree, environment);
		// }
	}

}


void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	bool update = false;
	update |= ImGui::SliderFloat("Persistance", &terrain.persistency, 0.1f, 0.6f);
	update |= ImGui::SliderFloat("Frequency gain", &terrain.frequency_gain, 1.5f, 2.5f);
	update |= ImGui::SliderInt("Octave", &terrain.octave, 1, 8);
	update |= ImGui::SliderFloat("Height", &terrain.height, 0.f, 10.f);

	if (update)// if any slider has been changed - then update the terrain
	{
		terrain.update_positions();	// Update step: Allows to update a mesh_drawable without creating a new one
		terrain_mesh.vbo_position.update(terrain.mesh.position);
		terrain_mesh.vbo_normal.update(terrain.mesh.normal);
		terrain_mesh.vbo_color.update(terrain.mesh.color);
		
		// for (vec3& pos: tree_position)
		// 	pos.z = terrain.evaluate_terrain_height(pos.x, pos.y);
		// tree.supplementary_vbo[0].clear();
		// tree.initialize_supplementary_data_on_gpu(cgp::numarray<vec3>(tree_position), 4, 1);
	}
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

void scene_structure::display_info()
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
}
