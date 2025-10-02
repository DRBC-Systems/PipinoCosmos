import { Test, TestingModule } from '@nestjs/testing';
import { UsersController } from './users.controller';
import { UsersService } from './users.service';
import * as fs from 'fs';

describe('UsersController', () => {
  let userController: UsersController;

  beforeEach(async () => {
    const app: TestingModule = await Test.createTestingModule({
      controllers: [UsersController],
      providers: [UsersService],
    }).compile();

    userController = app.get<UsersController>(UsersController);
  });

  describe('root', () => {
    it('should return an array of X elements', () => {
      const filePath = process.cwd() + '/data/users.json';
      const data = JSON.parse(fs.readFileSync(filePath).toString());

      expect(userController.getList()).toHaveLength(data.length);
    });
  });
});
