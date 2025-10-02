import { Controller, Get, HttpException, HttpStatus } from '@nestjs/common';
import { UsersService } from './users.service';
import { User } from './user.model';
import { ApiOkResponse, ApiOperation, getSchemaPath, ApiExtraModels, ApiResponse } from '@nestjs/swagger';

@Controller('users')
@ApiExtraModels(User)
export class UsersController {
  constructor(private readonly usersService: UsersService) {}

  @Get()
  @ApiOperation({ summary: 'Retrieve the list of available users.' })
  @ApiOkResponse({
    description: 'The array of available users.',
    schema: {
      type: 'array',
      items: { $ref: getSchemaPath(User) },
    },
  })
  @ApiResponse({ status: 500, description: 'Internal server error.'})
  getList(): User[] {
    const usersList = this.usersService.getList();

    if (usersList === null) {
      throw new HttpException('Internal server error', HttpStatus.INTERNAL_SERVER_ERROR);
    }

    return usersList;
  }
}
