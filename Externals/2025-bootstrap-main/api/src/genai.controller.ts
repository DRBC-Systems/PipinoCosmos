import { Controller, Get, Post, HttpException, HttpStatus, Query, Res, Body } from '@nestjs/common';
import { GenAIService } from './genai.service';
import { ApiOkResponse, ApiOperation, ApiResponse } from '@nestjs/swagger';
import { Response } from 'express';
import { ChatDto } from "./chat.dto";
import {TYPES} from "./message.dto";

@Controller('genai')
export class GenAIController {
  constructor(private readonly genAIService: GenAIService) {}

  @Get('prompt')
  @ApiOperation({ summary: 'Exchange a message with a GenAI model.' })
  @ApiOkResponse({
    description: 'The response from the model.'
  })
  @ApiResponse({ status: 500, description: 'Internal server error.'})
  async prompt(@Query('message') message: string): Promise<string> {
    const response = await this.genAIService.prompt(message);

    if (response === null) {
      throw new HttpException('Internal server error', HttpStatus.INTERNAL_SERVER_ERROR);
    }

    return response;
  }

  @Get('stream')
  @ApiOperation({ summary: 'Exchange a stream with a GenAI model.' })
  @ApiOkResponse({
    description: 'The response from the model.'
  })
  @ApiResponse({ status: 500, description: 'Internal server error.'})
  async stream(@Query('message') message: string, @Res() res: Response) {
    /**
     * This is just an example, you will need to use SSE events to stream the response
     * back to the actual client.
     *
     * https://docs.nestjs.com/techniques/events
     * https://docs.nestjs.com/techniques/server-sent-events
     */
    const stream = await this.genAIService.stream(message);

    if (stream === null) {
      throw new HttpException('Internal server error', HttpStatus.INTERNAL_SERVER_ERROR);
    }

    for await (const part of stream) {
      res.write(part.content.toString());
    }

    res.end();
  }

  @Post('chat')
  @ApiOperation({ summary: 'Exchange a chat with a GenAI model.' })
  @ApiOkResponse({
    description: 'The response from the model.'
  })
  @ApiResponse({ status: 500, description: 'Internal server error.'})
  async chat(@Body() chatDto: ChatDto): Promise<string> {
    /**
     * For something more complex see:
     * - https://js.langchain.com/docs/concepts/chat_history
     * - https://js.langchain.com/docs/concepts/prompt_templates
     */
    if (
        !chatDto ||
        !chatDto.messages ||
        chatDto.messages.length === 0 ||
        chatDto.messages[0].type !== TYPES.HUMAN
    ) {
      throw new HttpException('Bad request', HttpStatus.BAD_REQUEST);
    }

    const response = await this.genAIService.chat(chatDto);

    if (response === null) {
      throw new HttpException('Internal server error', HttpStatus.INTERNAL_SERVER_ERROR);
    }

    return response;
  }

  @Get('embeddings')
  @ApiOperation({ summary: 'Exchange a message with an Embeddings model.' })
  @ApiOkResponse({
    description: 'The response from the model.'
  })
  @ApiResponse({ status: 500, description: 'Internal server error.'})
  async embeddings(@Query('message') message: string): Promise<string> {
    const response = await this.genAIService.embeddings(message);

    if (response === null) {
      throw new HttpException('Internal server error', HttpStatus.INTERNAL_SERVER_ERROR);
    }

    return response;
  }

  @Get('rag')
  @ApiOperation({ summary: 'Exchange a message with an GenAI model using RAG.' })
  @ApiOkResponse({
    description: 'The response from the model.'
  })
  @ApiResponse({ status: 500, description: 'Internal server error.'})
  async rag(@Query('message') message: string): Promise<string> {
    const response = await this.genAIService.rag(message);

    if (response === null) {
      throw new HttpException('Internal server error', HttpStatus.INTERNAL_SERVER_ERROR);
    }

    return response;
  }
}
