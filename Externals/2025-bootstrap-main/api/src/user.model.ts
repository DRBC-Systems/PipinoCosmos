import { ApiSchema, ApiProperty } from '@nestjs/swagger';

@ApiSchema({ description: 'The User model schema' })
export class User {
    @ApiProperty({
        description: 'The id of the user',
        default: 1,
    })
    public id: number;

    @ApiProperty({
        description: 'The first name of the user',
        default: 'John',
    })
    public firstName: string;

    @ApiProperty({
        description: 'The last name of the user',
        default: 'Doe',
    })
    public lastName: string;

    constructor(id: number, firstName: string, lastName: string) {
        this.id = id;
        this.firstName = firstName;
        this.lastName = lastName;
    }
}
