import { ApiSchema, ApiProperty } from '@nestjs/swagger';
import {MessageDto} from "./message.dto";

@ApiSchema({ description: 'The Chat model schema' })
export class ChatDto {
    @ApiProperty({
        description: 'The messages exchanged in an array',
        type: [MessageDto]
    })
    public messages: Array<MessageDto>;
}
