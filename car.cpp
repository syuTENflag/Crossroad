#include "car.h"

// ���f���n���h��
int car::model_rcar;
int car::model_lcar;
int car::model_rora;

car::car() {
	// ���f���n���h���̓ǂݍ���
	model_rcar = MV1LoadModel("./resorces/veneno.pmx");
	model_lcar = MV1LoadModel("./resorces/veneno.pmx");
	model_rora = MV1LoadModel("./resorces/rora.mv1");


	initialize(CAR_TYPE_RCAR, 900.0f, VGet(2000.0f, 100.0f, -1350.0f), -50.0f);
}

car::car(int type, VECTOR pos, float v) {
	car_type = type;
	switch (type) {
	case CAR_TYPE_RCAR:
		initialize(CAR_TYPE_RCAR, 9.0f, pos, v);
		model_rotation = VGet(0.0f,  -DX_PI_F / 2, 0.0f);  //3D���f���̉�]
		break;
	case CAR_TYPE_LCAR:
		initialize(CAR_TYPE_LCAR, 9.0f, pos, v);
		model_rotation = VGet(0.0f, DX_PI_F / 2, 0.0f);
		break;
	case CAR_TYPE_RORA:
		initialize(CAR_TYPE_RORA, 3.0f, pos, v);
		model_rotation = VGet(DX_PI_F / 2, 0.0f, 0.0f);
		break;
	}
}

// ���������\�b�h
void car::initialize(int type, float extendf, VECTOR pos, float v) {
	// 3D���f���̓ǂݍ���(����)
	switch (type) {
	case CAR_TYPE_RCAR: model_handle = MV1DuplicateModel(model_rcar); break;
	case CAR_TYPE_LCAR:	model_handle = MV1DuplicateModel(model_lcar); break;
	case CAR_TYPE_RORA:	model_handle = MV1DuplicateModel(model_rora); break;
	}
	model_extend = VGet(extendf, extendf, extendf);		// 3D���f���̏k�ڗ��̊i�[
	flag = true;	// �L���ɂ���

	speed = v;		// �ړ��ʂ̐ݒ�

	/* ----- 3D���f���̐ݒ�ύX ----- */
	// 3D���f���̊g��k��
	MV1SetScale(model_handle, model_extend);

	// 3D���f���̗֊s���̏C��
	int MaterialNum = MV1GetMaterialNum(model_handle);
	for (int i = 0; i < MaterialNum; i++) {
		float dotwidth = MV1GetMaterialOutLineDotWidth(model_handle, i);	// �}�e���A���̗֊s���̑������擾  
		MV1SetMaterialOutLineDotWidth(model_handle, i, dotwidth / 50.0f);	// �}�e���A���̗֊s���̑������g�債��������������  
	}

	/* ----- 3D���f���̔z�u ���_(320.0f, -300.0f, 600.0f)�Ƃ��� ----- */
	model_position = VGet(pos.x, -300.0f + pos.y, pos.z);		// 3D���f���̍��W�̊i�[
	model_rotation = VGet(0.0f, 0.0f, 0.0f);					// 3D���f���̉�]�l�̏�����
	MV1SetPosition(model_handle, model_position);				// 3D���f����3D��Ԃւ̔z�u
}

// �X�V���\�b�h
void car::update() {
	// �ړ��ʌv�Z�p�̕ϐ�
	VECTOR value = VGet(speed, 0.0f, 0.0f);

	// �ړ��ʂ̉��Z
	model_position = VAdd(model_position, value);

	// ����A�����ɓ��B�����Ƃ��L���t���O�𗎂�
	if (model_position.x > ROAD_LIMIT_LEFT || ROAD_LIMIT_RIGHT > model_position.x) {
		flag = false;
	}

	// �e�ԃ^�C�v���Ƃ̏���
	switch (car_type) {
	case CAR_TYPE_RORA:	model_rotation.z -= speed / 1000.0f; break;
	}

	// �ړ���̍��W�ōĔz�u
	MV1SetPosition(model_handle, model_position);
	MV1SetRotationXYZ(model_handle, model_rotation);
}

// �`�惁�\�b�h
void car::draw() {
	// �R�c���f����`��
	MV1DrawModel(model_handle);

	// ���L�̂悤�ɃR�����g�A�E�g���Ă����`����𕜊������邱�Ƃ��ł��܂�
	// DrawFormatString(10, 250, GetColor(255, 255, 255), "Pos(%.2f, %.2f, %.2f)\n", model_position.x, model_position.y, model_position.z);		// 3D���f���̋�ԍ��W
	// DrawFormatString(10, 270, GetColor(255, 255, 255), "move_value[%.2f]\n", speed);
}

// ���O�\�����\�b�h
void car::draw_log() {
	printfDx("car_type[REGULAR] _handle[%d]\n", model_handle);
}

// �I���������\�b�h
void car::finalize() {
	// ���f���n���h���̍폜
	MV1DeleteModel(model_handle);
}

// �Ԃ̍��W�� VECTOR �^�Ŏ擾����
VECTOR car::get_position() {
	return model_position;
}

// �Ԃ̈ړ��ʂ� VECTOR �^�Ŏ擾����
VECTOR car::get_move_vector() {
	return VGet(speed, 0.0f, 0.0f);
}
