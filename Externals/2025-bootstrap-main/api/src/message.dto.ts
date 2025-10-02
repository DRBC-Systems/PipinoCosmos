import { ApiSchema, ApiProperty } from '@nestjs/swagger';

export enum TYPES
{
    SYSTEM = "System",
    HUMAN = "Human",
}

@ApiSchema({ description: 'The Message model schema' })
export class MessageDto {
    @ApiProperty({
        description: 'The type of the message',
        default: TYPES.HUMAN,
    })
    public type: TYPES;

    @ApiProperty({
        description: 'The actual text of the message',
        default: 'Hi, I will be attending the FuturEd AI hackathon in Crete',
    })
    public text: string;
}
